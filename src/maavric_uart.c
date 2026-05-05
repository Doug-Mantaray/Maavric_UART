/**
 * @file  maavric_uart.c
 *
 * @brief UART User Interface (TUI) for Maavric Avionics ACB Uart Streaming
 *
 * @details
 * 
 * - User interface showing individual DAQ inputs
 * - Receives Maavric Avionics Control Board (ACB) streaming data
 * 
 * Features:
 * - Provides status
 * 
 * @copyright Copyright (c) 2024-2025, Benchmark Space Systems
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <time.h>
#include <sys/poll.h>
#include <ncurses.h>

#include "serial.h"

#define BSS_HIL

//#define DAQ_CHANNELS  64
#define DAQ_CHANNELS  16

// starting location for all
#define DAQ_LOCX 4
#define DAQ_LOCY 1

// determine # of DAQ rows/cols
#if (DAQ_CHANNELS == 64)
#define DAQ_ROWS      4
#define DAQ_COLS      16
#endif  // (DAQ_CHANNELS == 64)
#if (DAQ_CHANNELS == 16)
#define DAQ_ROWS      2
#define DAQ_COLS      8
#endif  // (DAQ_CHANNELS == 16)

#ifdef  BSS_HIL
/***
 Example Display:

 DAQ┌--RTD--┐   ┌PT-P-01┐   ┌PT-F-01┐   ┌PT-F-02┐   ┌PT-F-03┐   ┌PT-M-01┐   ┌T-TM-01┐   ┌T-F-04-┐
   0│     0 │  1│     0 │  2│     0 │  3│     0 │  4│     0 │  5│     0 │  6│     0 │  7│     0 │
    └───────┘   └───────┘   └───────┘   └───────┘   └───────┘   └───────┘   └───────┘   └───────┘
    ┌T-F-05-┐   ┌T-F-06-┐   ┌T-RCS-1┐   ┌T-RCS-2┐   ┌T-RCS-3┐   ┌T-RCS-4┐   ┌T-F-01-┐   ┌T-F-02-┐
   8│     0 │  9│     0 │ 10│     0 │ 11│     0 │ 12│     0 │ 13│     0 │ 14│     0 │ 15│     0 │
    └───────┘   └───────┘   └───────┘   └───────┘   └───────┘   └───────┘   └───────┘   └───────┘


    F1/Ctrl-C to exit
***/
static const char *daq_ids[DAQ_CHANNELS] =
    { "--RTD--", "PT-P-01", "PT-F-01", "PT-F-02", "PT-F-03", "PT-M-01", "T-TM-01", "T-F-04-",
      "T-F-05-", "T-F-06-", "T-RCS-1", "T-RCS-2", "T-RCS-3", "T-RCS-4", "T-F-01-", "T-F-02-" };
#else   // BSS_HIL
#endif  // BSS_HIL

WINDOW  *daq_info[DAQ_CHANNELS];
uint16_t daq_data[DAQ_CHANNELS];
uint16_t daq_prev[DAQ_CHANNELS];


void config_usb_params( void );
void intHandler( int dummy );
static volatile int keepRunning = 1;

// todo: determine device name for zed board (might change to /dev/ttyPS1)
const char *device_name = "/dev/ttyUSB0";
int32_t port_status = -1;


/**
 * @brief   main (duh)
 * 
 * @param   argc 
 * @param   argv 
 * @return  int 
 */
int main(int argc, char *argv[])
{
    int rows = 1;
    int cols = 1;

    signal(SIGINT, intHandler);

    config_usb_params();
    port_status = serial_port_init( device_name, PORT_BAUD_RATE );
    printf("Serial port: %c\n", port_status == 0? 'Y':'N');

//setlocale(LC_ALL, "");
    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    /* Initialize few color pairs */
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_BLACK, COLOR_CYAN);

    #define DAQ_BOX_SIZE 9
    // daq values
    for (int daq = 0; daq < DAQ_CHANNELS; daq++)
    {
        int row = daq / DAQ_COLS;
        int col = daq % DAQ_COLS;
#if     (DAQ_COLS > 8)
        int loc_x = 1 + (col*DAQ_BOX_SIZE)%(DAQ_COLS*10);
#else   // DAQ_COLS > 8
        int loc_x = 4 + (col*(DAQ_BOX_SIZE+3))%(DAQ_COLS*12);
#endif  // DAQ_COLS > 8
        int loc_y = (1 + rows + 1) + row*3;

        // show some daq labels
        if (col == 0)
        {
            if (row == 0)
            {
                mvprintw(loc_y, loc_x-3, "DAQ");
            }
#if     (DAQ_COLS > 8)
            // label row
            mvprintw(loc_y+1, loc_x-3, "%2d:", row*DAQ_COLS);
#endif  // DAQ_COLS > 8
        }

#if     (DAQ_COLS <= 8)
        // number each box (outside the window)
        mvprintw(loc_y+1, loc_x-2, "%2d", row*DAQ_COLS+col);
        refresh();
#endif  // DAQ_COLS > 8

        daq_info[daq] = newwin(3, DAQ_BOX_SIZE, loc_y, loc_x);
        box(daq_info[daq], 0, 0);
#ifdef  BSS_HIL
        // label each box (within the window)
        mvwprintw(daq_info[daq], 0, 1, daq_ids[row*DAQ_COLS+col]);
#endif  // BSS_HIL
        wrefresh(daq_info[daq]);
        nodelay(daq_info[daq], TRUE);

        // init previous value to unlikely value to trigger change for initial display
        daq_prev[daq] = 0xffff-1;
    }

    nodelay(stdscr, TRUE);

    mvprintw(LINES - 2, 4, "F1/Ctrl-C to exit");
    refresh();

    // misc reference timers
#ifdef  SYNC_CHARS
    // note: using sync characters will reset rx queue and not be counted
    uint32_t length = 32;
#else   // SYNC_CHARS
    uint32_t length = 34;
#endif  // SYNC_CHARS

    while (keepRunning)
    {
        if ( port_status == 0 )
        {
            serial_port_queue();

            // read/decode serial info & display
            uint8_t data[1024];
            if (serial_port_qsize() >= length)
            {
                serial_port_read( data, length );
                for (int daq = 0; daq < DAQ_CHANNELS; daq++)
                {
#ifdef SYNC_DELAY
                    daq_data[daq] = (uint16_t ) data[daq*2+2] << 8 | (uint16_t ) data[daq*2+3];
#else   // SYNC_DELAY
                    daq_data[daq] = (uint16_t ) data[daq*2] << 8 | (uint16_t ) data[daq*2+1];
#endif  // SYNC_DELAY
                    // update on change
                    if (daq_prev[daq] != daq_data[daq])
                    {
                        mvwprintw(daq_info[daq], 1, 1, "%6d", daq_data[daq]);
                        //mvwprintw(daq_info[daq], 1, 1, "%04x", daq_data[daq]);
                        wrefresh(daq_info[daq]);
            
                        daq_prev[daq] = daq_data[daq];
                    }
                }
            }
        }

       /*
        * Process keypresses
        */
        //int ch = wgetch(mcu_table[mcu].mcu_win);
        int ch = wgetch(stdscr);
        if (ch == ERR)
        {
            // no key pressed
            continue;
        }
        if (ch == KEY_F(1))
        {
            // exit
            break;
        }
    }

    endwin();

    if ( port_status == 0 )
    {
        serial_port_close();
    }

    printf("bye...\n");
    return 0;
}


/**
 * @brief Configure USB serial port parameters to shorten delays
 * 
 */
void config_usb_params( void )
{
#define LATENCY_TIMER   "/sys/bus/usb-serial/devices/ttyUSB0/latency_timer"
//printf("cmd: " "sudo bash -c \"echo 1 > %s\"\n", LATENCY_TIMER);
    // note: when using a USB to RS-422 adapter, changing the latency timer can shorten read times
    FILE *fd_val = fopen(LATENCY_TIMER, "r");
    if (fd_val != NULL)
    {
        uint32_t val;
        fscanf(fd_val, "%d", &val);
        fclose(fd_val);

        if (val > 1)
        {
            char buf[256];
#ifdef BUILT_BY_ARM
            sprintf(buf, "echo 1 > " LATENCY_TIMER);
#else  // BUILT_BY_ARM
            sprintf(buf, "sudo bash -c \"echo 1 > %s\"", LATENCY_TIMER);
            //system("sudo bash -c \"echo 1 > /sys/bus/usb-serial/devices/ttyUSB0/latency_timer\"");
#endif // BUILT_BY_ARM
            system(buf);
        }
    }
}


/**
 * @brief Ctrl-c interrupt handler
 * 
 * @param dummy 
 */
void intHandler( int dummy )
{
    printf("\nctrl-c intHandler\n");
    keepRunning = 0;
}
