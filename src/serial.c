/**
 * @file  serial.c
 *
 * @brief (Mostly) generic Linux serial interface
 *
 * @details
 * 
 * - Provides serial initialization & read/write routines
 * 
 * @copyright Copyright (c) 2023-2024, Benchmark Space Systems
 * 
 */

#include "serial.h"

struct pollfd fd_serial[1];
static uint8_t  rx_queue[MAX_DATA_QUEUE];
static uint32_t rx_head = 0;
static uint32_t rx_tail = 0;


/**
 * @brief  Initialize serial port to read/write
 * 
 * @param   device_name 
 * @param   baud_rate 
 * @return  int32_t 
 */
int32_t serial_port_init( const char *device_name, uint32_t baud_rate )
{
    int port_fd = open(device_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (port_fd >= 0)
    {
        fcntl(port_fd, F_SETFL, FNDELAY); // make read command non blocking

        // Create new termios struct
        struct termios tty;

        // Read in existing settings, and handle any error
        // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
        // must have been initialized with a call to tcgetattr() overwise behaviour is undefined
        if(tcgetattr(port_fd, &tty) != 0)
        {
            return -1;
        }
        tty.c_cflag &= ~PARENB; // Disable parity
        tty.c_cflag &= ~CSTOPB; // 1 stop bit
        tty.c_cflag &= ~CSIZE;   // Clear all the size bits
        tty.c_cflag |= CS8;     // 8 bits per byte
        tty.c_cflag &= ~CRTSCTS; // no flow control
        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
        tty.c_lflag &= ~ICANON; // Disable canonical mode, important for sending/receiving raw bytes
        tty.c_lflag &= ~ECHO;   // Disable echo
        tty.c_lflag &= ~ECHOE;  // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG; // Disable interpretation of special chars INTR, QUIT and SUSP
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off flow ctrl
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR; // Don't do LF -> CR/LF

        tty.c_cc[VTIME] = 0;    //  (0 deciseconds), dont block
        tty.c_cc[VMIN] = 0;     // just return any data available

        // Set a default baud rate initially
        switch (baud_rate)
        {
            case 38400:
                cfsetspeed(&tty, B38400);
                break;
            case 921600:
                cfsetspeed(&tty, B921600);
                break;
            default:
                cfsetspeed(&tty, B115200);
                break;
        }

        // Save tty settings, also checking for error
        if (tcsetattr(port_fd, TCSANOW, &tty) != 0)
        {
            return -1;
        }

        fd_serial[0].fd = port_fd;
        fd_serial[0].events = POLLIN;

        tcflush(fd_serial[0].fd, TCIOFLUSH);
        rx_head = 0;
        rx_tail = 0;
        return 0;
    }

    return -1;
}


/**
 * @brief  Close serial port
 * 
 * @return int32_t 
 */
int32_t serial_port_close( void )
{
    return close(fd_serial[0].fd);
}


/**
 * @brief   Read serial port data & add to queue
 * 
 * @param   N/A
 * @return  int32_t 
 */
int32_t serial_port_queue( void )
{
    const struct timespec timeout_ts =
    {
     // sec  nsec
#ifdef  SYNC_DELAY
        0,   SYNC_DELAY
#else   // SYNC_DELAY
        0,   1000000
#endif  // SYNC_DELAY
    };
//    int pollrc = poll(fd_serial, 1, 0);
    int pollrc = ppoll(fd_serial, 1, &timeout_ts, NULL);

    if (pollrc < 0)
    {
        return -1;
    }
    else if (pollrc == 0)
    {
        // timeout, reset queue
#ifdef SYNC_DELAY
        // resetting kinda works with USB receiving ACB data
        rx_head = rx_tail = 0;
#endif  // SYNC_DELAY
    }
    else if ( pollrc > 0
          && (fd_serial[0].revents & POLLIN ) )
    {
        uint8_t buf[MAX_DATA_QUEUE];
        int  num_bytes = read(fd_serial[0].fd, buf, MAX_DATA_QUEUE);

        // store new data in queue
        for (int i = 0; i < num_bytes; i++)
        {
#ifdef  SYNC_CHARS
            // clear queue at beginning (0xff 0xff)
            if (buf[i] == 0xff)
            {
                rx_head = rx_tail = 0;
                continue;
            }
#endif  // SYNC_CHARS
            if ( (rx_head + 1) % MAX_DATA_QUEUE == rx_tail )
            {
                // queue is full, drop remaining data
                return -1;
            }
            rx_queue[rx_head++] = (uint8_t) buf[i];
            rx_head %= MAX_DATA_QUEUE;
        }
    }

    return 0;
}


/**
 * @brief  Write data to serial port
 * 
 * @param   data 
 * @param   length 
 * @return  int32_t 
 */
int32_t serial_port_write( uint8_t *data, uint32_t length )
{
    return write(fd_serial[0].fd, data, length);
}


/**
 * @brief  Check the rx queue size
 * 
 * @return int32_t 
 */
int32_t serial_port_qsize( void )
{
    return (( rx_head + MAX_DATA_QUEUE) - rx_tail) % MAX_DATA_QUEUE;
}


/**
 * @brief  Read data from rx queue
 * 
 * @return int32_t 
 */
int32_t serial_port_read( uint8_t *data, uint32_t length )
{
    uint32_t index = 0;

    while (rx_tail != rx_head
        && index < length)
    {
        data[index++] = rx_queue[rx_tail++];
        rx_tail %= MAX_DATA_QUEUE;
    }

    return index;
}


/**
 * @brief   Get the response from rx queue
 * 
 * @details
 * Suck data off queue until end of line (EOL) is reached
 * Don't suck if EOL isn't found
 * 
 * @param   data 
 * @param   eol 
 * @return  length
 */
int32_t get_response( uint8_t *data, uint8_t eol )
{
    uint32_t tail = rx_tail;
    uint32_t index = 0;

    while (tail != rx_head )
    {
        data[index++] = rx_queue[tail++];
        tail %= MAX_DATA_QUEUE;
        if (data[index-1] == eol) // '>'
        {
            // end of line, suck off queue & return length of command
            rx_tail = tail;
            return index;
        }
    }

    return 0;
}
