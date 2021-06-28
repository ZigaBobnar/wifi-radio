#ifndef FIFO_H_
#define FIFO_H_

#include "common.h"

/**
 * FIFO (First In, First Out) buffer
 * 
 * Fifo cyclic buffer can be used whenewer a program needs to always append
 * data after the current position in memory without the need for infinite
 * memory size. The buffer can be read all the way to the most recently written
 * data. Existing buffer data must be read (read index advances) before it can
 * be overwritten.
 */

__EXTERN_C_BEGIN

struct _fifo {
    uint32_t read_idx;
    uint32_t write_idx;
    uint32_t size;
    uint8_t* buffer;
};
typedef struct _fifo fifo_t;

/**
 * Read from fifo buffer
 * @param fifo Fifo buffer to read data from
 * @param data_buffer Pointer to where fifo data should be read
 * @param n Number of bytes to read from buffer
 * @returns Number of bytes that were read
 */
uint32_t fifo_read(fifo_t* fifo, uint8_t* data_buffer, uint32_t n);

/**
 * Peeks (reads without advancing read pointer) the fifo buffer
 * @param fifo Fifo buffer to read data from
 * @param data_buffer Pointer to where fifo data should be read
 * @param n Number of bytes to read from buffer
 * @returns Number of bytes that were read
 */
uint32_t fifo_peek(fifo_t* fifo, uint8_t* data_buffer, uint32_t n);

/**
 * Checks whether one more item is available to be read from fifo buffer
 * @param fifo Fifo buffer to check
 * @returns True if at least one item can be read from fifo
 */
bool fifo_has_next_item(fifo_t* fifo);

/**
 * Write data into fifo buffer
 * @param fifo Fifo buffer to write data to
 * @param data_ptr Pointer to data that needs to be written into fifo buffer
 * @param n Number of bytes to write to buffer
 * @returns Number of bytes that were written
 */
uint32_t fifo_write(fifo_t* fifo, uint8_t* data_ptr, uint32_t n);

/**
 * Write single byte into buffer
 * @param fifo Fifo buffer to write data to
 * @param value Value that needs to be written into fifo buffer
 * @returns Number of bytes that were written
 */
uint32_t fifo_write_single(fifo_t* fifo, uint8_t value);

/**
 * Discard any unread data
 * This will set the read and write indexes to zero to reset fifo
 * @param fifo Fifo buffer to discard
 */
void fifo_discard(fifo_t* fifo);

__EXTERN_C_END

#endif  // FIFO_H_
