/**
 * @file  serial.h
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

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <termios.h>


#define SYNC_DELAY      2000000
//#define SYNC_CHARS      0xff
#define PORT_BAUD_RATE  921600

#define MAX_DATA_QUEUE  1024

int32_t serial_port_init(  const char *device_name, uint32_t baud_rate  );
int32_t serial_port_close( void );
int32_t serial_port_queue( void );
int32_t serial_port_qsize( void );
int32_t serial_port_read( uint8_t *data, uint32_t length );
int32_t serial_port_write( uint8_t *data, uint32_t length );
int32_t get_response( uint8_t *data, uint8_t eol );
