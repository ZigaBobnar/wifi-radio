/*
 * asf.h
 * 
 * This file contains includes for the ASF library.
 * 
 */

#ifndef ASF_H_
#define ASF_H_

/*
 * This file includes all API header files for the selected drivers from ASF.
 * Note: There might be duplicate includes required by more than one driver.
 *
 */
#include <sam3x8e.h>
#include <arduino_due_x/arduino_due_x.h>
#include <pio.h>
#include <delay.h>

// From module: Common SAM compiler driver
#include <compiler.h>
#include <status_codes.h>

// From module: GPIO - General purpose Input/Output
//#include <gpio.h>

// From module: Generic board support
//#include <board.h>

// From module: IOPORT - General purpose I/O service
#include <ioport.h>

// From module: Interrupt management - SAM implementation
//#include <interrupt.h>

// From module: PIO - Parallel Input/Output Controller
//#include <pio.h>

// From module: PMC - Power Management Controller
//#include <pmc.h>
//#include <sleep.h>

// From module: Part identification macros
//#include <parts.h>

// From module: SAM3X startup code
//#include <exceptions.h>

// From module: Standard serial I/O (stdio) - SAM implementation
//#include <stdio_serial.h>

// From module: System Clock Control - SAM3X/A implementation
#include <sysclk.h>

// From module: TC - Timer Counter
#include <tc.h>

// From module: UART - Univ. Async Rec/Trans
#include <uart.h>

// From module: USART - Serial interface - SAM implementation for devices with both UART and USART
//#include <serial.h>

// From module: USART - Univ. Syn Async Rec/Trans
#include <usart.h>

// From module: pio_handler support enabled
//#include <pio_handler.h>


#endif  // ASF_H_
