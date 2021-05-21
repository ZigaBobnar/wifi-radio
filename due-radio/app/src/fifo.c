#include "fifo.h"

__EXTERN_C_BEGIN

uint32_t fifo_read(fifo_t* fifo, uint8_t* data_buffer, uint32_t n) {
    uint32_t bytes_read = 0;

    while (bytes_read < n && fifo->read_idx != fifo->write_idx) {
        if (fifo->read_idx >= fifo->size) {
            fifo->read_idx = 0;
        }

        *(data_buffer + bytes_read) = *(fifo->buffer + fifo->read_idx);
        fifo->read_idx++;

        bytes_read++;
    }

    return bytes_read;
}

uint32_t fifo_peek(fifo_t* fifo, uint8_t* data_buffer, uint32_t n) {
    uint32_t peek_idx = fifo->read_idx;
    uint32_t bytes_read = 0;

    while (bytes_read < n && peek_idx != fifo->write_idx) {
        if (peek_idx >= fifo->size) {
            peek_idx = 0;
        }

        *(data_buffer + bytes_read) = *(fifo->buffer + peek_idx);
        peek_idx++;

        bytes_read++;
    }

    return bytes_read;
}

bool fifo_has_next_item(fifo_t* fifo) {
    return (fifo->read_idx != fifo->write_idx);
}

uint32_t fifo_write(fifo_t* fifo, uint8_t* data_ptr, uint32_t n) {
    uint32_t bytes_written = 0;

    while (bytes_written < n && (fifo->write_idx != (fifo->read_idx - 1 + fifo->size) % fifo->size)) {
        if (fifo->write_idx >= fifo->size) {
            fifo->write_idx = 0;
        }

        *(fifo->buffer + fifo->write_idx) = *(data_ptr + bytes_written);
        fifo->write_idx++;

        bytes_written++;
    }

    return bytes_written;
}

uint32_t fifo_write_single(fifo_t* fifo, uint8_t value) {
    return fifo_write(fifo, &value, 1);
}

void fifo_discard(fifo_t* fifo) {
    fifo->read_idx = 0;
    fifo->write_idx = 0;
}

__EXTERN_C_END
