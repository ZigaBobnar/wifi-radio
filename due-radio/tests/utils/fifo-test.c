#include "test.h"
#include "due-radio/utils/fifo.h"

TEST_F(fifo_create_correctStructure) {
    fifo_t* fifo = fifo_create(23);

    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);
    TEST_ASSERT_EQUAL(23, fifo->buffer_end - fifo->buffer_start);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, fifo->buffer_start, 2);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_single_getsCorrectPreSetValue) {
    fifo_t* fifo = fifo_create(10);
    *(fifo->write_ptr++) = 244;
    uint8_t value;

    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(244, value);
    TEST_ASSERT_EQUAL_PTR(fifo->read_ptr, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_single_noDataNoChange) {
    fifo_t* fifo = fifo_create(10);
    uint8_t value = 94;

    TEST_ASSERT_FALSE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(94, value);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_single_getsCorrectPreSetValues) {
    fifo_t* fifo = fifo_create(10);
    *(fifo->write_ptr++) = 244;
    *(fifo->write_ptr++) = 43;
    *(fifo->write_ptr++) = 94;
    uint8_t value;

    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(244, value);
    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(43, value);
    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(94, value);
    TEST_ASSERT_FALSE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(94, value);
    TEST_ASSERT_EQUAL_PTR(fifo->read_ptr, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_single_turnOver) {
    fifo_t* fifo = fifo_create(8);
    fifo->write_ptr = fifo->read_ptr = fifo->buffer_end - 1;
    *(fifo->write_ptr++) = 244;
    *(fifo->write_ptr) = 43;
    fifo->write_ptr = fifo->buffer_start;
    *(fifo->write_ptr++) = 94;
    *(fifo->write_ptr++) = 23;
    uint8_t value;

    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(244, value);
    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(43, value);
    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(94, value);
    TEST_ASSERT_TRUE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(23, value);
    TEST_ASSERT_FALSE(fifo_read_single(fifo, &value));
    TEST_ASSERT_EQUAL(23, value);
    TEST_ASSERT_EQUAL_PTR(fifo->read_ptr, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_single_skipOnNullBuffer) {
    fifo_t* fifo = fifo_create(10);
    *(fifo->write_ptr++) = 244;
    *(fifo->write_ptr++) = 43;

    TEST_ASSERT_TRUE(fifo_read_single(fifo, NULL));

    fifo_destroy(fifo);
}

TEST_F(fifo_write_single_fifoFull) {
    fifo_t* fifo = fifo_create(4);
    uint8_t* original_read = fifo->read_ptr += 2;
    uint8_t* original_write = fifo->write_ptr = fifo->read_ptr - 1;

    TEST_ASSERT_FALSE(fifo_write_single(fifo, 62));
    TEST_ASSERT_EACH_EQUAL_INT8(0, fifo->buffer_start, 4);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    
    fifo_destroy(fifo);
}

TEST_F(fifo_write_single_toStart) {
    fifo_t* fifo = fifo_create(6);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;

    TEST_ASSERT_TRUE(fifo_write_single(fifo, 62));
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write + 1, fifo->write_ptr);
    TEST_ASSERT_EQUAL(62, *fifo->read_ptr);
    TEST_ASSERT_EQUAL(0, *fifo->write_ptr);
    
    fifo_destroy(fifo);
}

TEST_F(fifo_write_single_toEnd) {
    fifo_t* fifo = fifo_create(6);
    uint8_t* original_read = fifo->read_ptr = fifo->write_ptr = fifo->buffer_end;

    TEST_ASSERT_TRUE(fifo_write_single(fifo, 62));
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);
    TEST_ASSERT_EQUAL(62, *fifo->read_ptr);
    TEST_ASSERT_EQUAL(0, *fifo->write_ptr);
    
    fifo_destroy(fifo);
}

TEST_F(fifo_write_single_multipleInMiddle) {
    fifo_t* fifo = fifo_create(12);
    uint8_t* original_read = fifo->read_ptr += 3;
    uint8_t* original_write = fifo->write_ptr = original_read;

    TEST_ASSERT_TRUE(fifo_write_single(fifo, 62));
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write + 1, fifo->write_ptr);
    TEST_ASSERT_EQUAL(62, *fifo->read_ptr);
    TEST_ASSERT_EQUAL(0, *fifo->write_ptr);

    TEST_ASSERT_TRUE(fifo_write_single(fifo, 21));
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write + 2, fifo->write_ptr);
    TEST_ASSERT_EQUAL(62, *fifo->read_ptr);
    TEST_ASSERT_EQUAL(21, *(fifo->read_ptr + 1));
    TEST_ASSERT_EQUAL(0, *fifo->write_ptr);
    
    fifo_destroy(fifo);
}

TEST_F(fifo_write_single_multipleOnEndRollover) {
    fifo_t* fifo = fifo_create(12);
    uint8_t* original_read = fifo->read_ptr = fifo->write_ptr = fifo->buffer_end - 1;

    TEST_ASSERT_TRUE(fifo_write_single(fifo, 62));
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_end, fifo->write_ptr);
    TEST_ASSERT_EQUAL(62, *fifo->read_ptr);
    TEST_ASSERT_EQUAL(0, *fifo->write_ptr);

    TEST_ASSERT_TRUE(fifo_write_single(fifo, 21));
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);
    TEST_ASSERT_EQUAL(62, *fifo->read_ptr);
    TEST_ASSERT_EQUAL(21, *(fifo->read_ptr + 1));
    TEST_ASSERT_EQUAL(0, *fifo->write_ptr);
    
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 94));
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 1, fifo->write_ptr);
    TEST_ASSERT_EQUAL(62, *fifo->read_ptr);
    TEST_ASSERT_EQUAL(21, *(fifo->read_ptr + 1));
    TEST_ASSERT_EQUAL(94, *(fifo->write_ptr - 1));
    TEST_ASSERT_EQUAL(0, *fifo->write_ptr);
    
    fifo_destroy(fifo);
}

TEST_F(fifo_write_single_overflowSmallBufferReturnsFalse) {
    fifo_t* fifo = fifo_create(4);
    
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 62));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 35));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 15));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 46));
    TEST_ASSERT_FALSE(fifo_write_single(fifo, 46));
    
    fifo_destroy(fifo);
}

TEST_F(fifo_valid_checkValidManual) {
    uint8_t* data_buffer = (uint8_t*)malloc(sizeof(uint8_t) * 17);

    fifo_t fifo = {
        .buffer_start = data_buffer,
        .buffer_end = data_buffer + 16,
        .read_ptr = data_buffer,
        .write_ptr = data_buffer,
    };

    TEST_ASSERT_TRUE(fifo_valid(&fifo));

    free(data_buffer);
}

TEST_F(fifo_valid_checkValidNew) {
    fifo_t* fifo = fifo_create(16);

    TEST_ASSERT_TRUE(fifo_valid(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_valid_invalidAfterDestroy) {
    fifo_t* fifo = fifo_create(16);
    fifo_destroy(fifo);

    TEST_ASSERT_FALSE(fifo_valid(fifo));
}

TEST_F(fifo_valid_invalidWritePtr) {
    fifo_t* fifo = fifo_create(16);

    fifo->write_ptr = fifo->buffer_end + 1;

    TEST_ASSERT_FALSE(fifo_valid(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_valid_invalidWritePtr2) {
    fifo_t* fifo = fifo_create(16);

    fifo->write_ptr = fifo->buffer_start - 1;

    TEST_ASSERT_FALSE(fifo_valid(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_valid_invalidReadPtr) {
    fifo_t* fifo = fifo_create(16);

    fifo->read_ptr = fifo->buffer_end + 1;

    TEST_ASSERT_FALSE(fifo_valid(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_valid_invalidReadPtr2) {
    fifo_t* fifo = fifo_create(16);

    fifo->write_ptr = fifo->buffer_start - 1;

    TEST_ASSERT_FALSE(fifo_valid(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_reset_resetsToBufferBegin) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 23);
    fifo_write_single(fifo, 22);
    fifo_read_single(fifo, NULL);

    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 2, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 1, fifo->read_ptr);

    fifo_reset(fifo);

    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_size_sameValueAsCreate) {
    fifo_t* fifo = fifo_create(16);

    TEST_ASSERT_EQUAL(16, fifo_size(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_single_peeksWithoutIncrementingReadPtr) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value;

    TEST_ASSERT_TRUE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(24, value);
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_single_peeksFalseWhenNoData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value = 32;

    TEST_ASSERT_FALSE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(32, value);
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_single_peekMultipleTimesEachTimeSame) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 93);
    fifo_write_single(fifo, 56);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value;

    TEST_ASSERT_TRUE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(24, value);
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);
    TEST_ASSERT_TRUE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(24, value);
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);
    TEST_ASSERT_TRUE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(24, value);
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_single_peekMultipleTimesReadMovingForward) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 93);
    fifo_write_single(fifo, 56);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value;

    TEST_ASSERT_TRUE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(24, value);
    TEST_ASSERT_EQUAL_PTR(original_read, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);
    TEST_ASSERT_TRUE(fifo_read_single(fifo, NULL));
    TEST_ASSERT_TRUE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(93, value);
    TEST_ASSERT_EQUAL_PTR(original_read + 1, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);
    TEST_ASSERT_TRUE(fifo_read_single(fifo, NULL));
    TEST_ASSERT_TRUE(fifo_peek_single(fifo, &value));
    TEST_ASSERT_EQUAL(56, value);
    TEST_ASSERT_EQUAL_PTR(original_read + 2, fifo->read_ptr);
    TEST_ASSERT_EQUAL_PTR(original_write, fifo->write_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_has_next_item_returnsTrueWhenAvailable) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 13);

    TEST_ASSERT_TRUE(fifo_has_next_item(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_has_next_item_returnsFalseWhenNotAvailable) {
    fifo_t* fifo = fifo_create(16);
    
    TEST_ASSERT_FALSE(fifo_has_next_item(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_has_next_item_returnsFalseWhenAlreadyRead) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_read_single(fifo, NULL);
    
    TEST_ASSERT_FALSE(fifo_has_next_item(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_is_full_returnsFalseOnEmpty) {
    fifo_t* fifo = fifo_create(16);

    TEST_ASSERT_FALSE(fifo_is_full(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_is_full_smallFifoOverflow) {
    fifo_t* fifo = fifo_create(3);
    
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 24));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 35));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 45));
    TEST_ASSERT_TRUE(fifo_is_full(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_is_full_smallFifoOverflow2) {
    fifo_t* fifo = fifo_create(3);
    
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 24));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 35));
    TEST_ASSERT_TRUE(fifo_read_single(fifo, NULL));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 45));
    TEST_ASSERT_TRUE(fifo_write_single(fifo, 235));
    TEST_ASSERT_TRUE(fifo_is_full(fifo));

    fifo_destroy(fifo);
}

TEST_F(fifo_read_readsNothingWhenNoData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(0, fifo_read(fifo, buf, 16));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_readsIntoBuffer) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(3, fifo_read(fifo, buf, 16));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 3, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 3, fifo->read_ptr);
    TEST_ASSERT_EQUAL(24, buf[0]);
    TEST_ASSERT_EQUAL(63, buf[1]);
    TEST_ASSERT_EQUAL(37, buf[2]);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 3, 16 - 3);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_readNothingLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(0, fifo_read(fifo, buf, 0));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 2, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_readLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    fifo_write_single(fifo, 75);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(4, fifo_read(fifo, buf, 4));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 5, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 4, fifo->read_ptr);
    TEST_ASSERT_EQUAL(24, buf[0]);
    TEST_ASSERT_EQUAL(63, buf[1]);
    TEST_ASSERT_EQUAL(37, buf[2]);
    TEST_ASSERT_EQUAL(35, buf[3]);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 4, 16 - 4);

    fifo_destroy(fifo);
}

TEST_F(fifo_read_readFull) {
    fifo_t* fifo = fifo_create(4);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    uint8_t buf[5] = { 0 };
    
    TEST_ASSERT_EQUAL(4, fifo_read(fifo, buf, 16));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_end, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_end, fifo->read_ptr);
    TEST_ASSERT_EQUAL(24, buf[0]);
    TEST_ASSERT_EQUAL(63, buf[1]);
    TEST_ASSERT_EQUAL(37, buf[2]);
    TEST_ASSERT_EQUAL(35, buf[3]);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_readsNothingWhenNoData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(0, fifo_peek(fifo, buf, 16));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_readsIntoBuffer) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(3, fifo_peek(fifo, buf, 16));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 3, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EQUAL(24, buf[0]);
    TEST_ASSERT_EQUAL(63, buf[1]);
    TEST_ASSERT_EQUAL(37, buf[2]);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 3, 16 - 3);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_readNothingLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(0, fifo_peek(fifo, buf, 0));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 2, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_readLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    fifo_write_single(fifo, 75);
    uint8_t buf[17] = { 0 };
    
    TEST_ASSERT_EQUAL(4, fifo_peek(fifo, buf, 4));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 5, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EQUAL(24, buf[0]);
    TEST_ASSERT_EQUAL(63, buf[1]);
    TEST_ASSERT_EQUAL(37, buf[2]);
    TEST_ASSERT_EQUAL(35, buf[3]);
    TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 4, 16 - 4);

    fifo_destroy(fifo);
}

TEST_F(fifo_peek_readFull) {
    fifo_t* fifo = fifo_create(4);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    uint8_t buf[5] = { 0 };
    
    TEST_ASSERT_EQUAL(4, fifo_peek(fifo, buf, 16));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_end, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EQUAL(24, buf[0]);
    TEST_ASSERT_EQUAL(63, buf[1]);
    TEST_ASSERT_EQUAL(37, buf[2]);
    TEST_ASSERT_EQUAL(35, buf[3]);

    fifo_destroy(fifo);
}

TEST_F(fifo_write_noDataToWrite) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[5] = { 0 };
    
    TEST_ASSERT_EQUAL(0, fifo_write(fifo, buf, 0));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);

    fifo_destroy(fifo);
}

TEST_F(fifo_write_writesAllData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[5] = { 34, 45, 46, 53 };
    
    TEST_ASSERT_EQUAL(4, fifo_write(fifo, buf, 4));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start + 4, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EQUAL(34, fifo->buffer_start[0]);
    TEST_ASSERT_EQUAL(45, fifo->buffer_start[1]);
    TEST_ASSERT_EQUAL(46, fifo->buffer_start[2]);
    TEST_ASSERT_EQUAL(53, fifo->buffer_start[3]);

    fifo_destroy(fifo);
}

TEST_F(fifo_write_smallBufferWriteUntilFull) {
    fifo_t* fifo = fifo_create(4);
    uint8_t buf[9] = { 34, 45, 46, 53, 57, 24, 41, 78 };
    
    TEST_ASSERT_EQUAL(4, fifo_write(fifo, buf, 9));
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_end, fifo->write_ptr);
    TEST_ASSERT_EQUAL_PTR(fifo->buffer_start, fifo->read_ptr);
    TEST_ASSERT_EQUAL(34, fifo->buffer_start[0]);
    TEST_ASSERT_EQUAL(45, fifo->buffer_start[1]);
    TEST_ASSERT_EQUAL(46, fifo->buffer_start[2]);
    TEST_ASSERT_EQUAL(53, fifo->buffer_start[3]);

    fifo_destroy(fifo);
}

void runFifoTests(void) {
    RUN_TEST(fifo_create_correctStructure);
    RUN_TEST(fifo_read_single_getsCorrectPreSetValue);
    RUN_TEST(fifo_read_single_noDataNoChange);
    RUN_TEST(fifo_read_single_getsCorrectPreSetValues);
    RUN_TEST(fifo_read_single_turnOver);
    RUN_TEST(fifo_read_single_skipOnNullBuffer);
    RUN_TEST(fifo_write_single_fifoFull);
    RUN_TEST(fifo_write_single_toStart);
    RUN_TEST(fifo_write_single_toEnd);
    RUN_TEST(fifo_write_single_multipleInMiddle);
    RUN_TEST(fifo_write_single_multipleOnEndRollover);
    RUN_TEST(fifo_write_single_overflowSmallBufferReturnsFalse);
    RUN_TEST(fifo_valid_checkValidManual);
    RUN_TEST(fifo_valid_checkValidNew);
    RUN_TEST(fifo_valid_invalidAfterDestroy);
    RUN_TEST(fifo_valid_invalidWritePtr);
    RUN_TEST(fifo_valid_invalidWritePtr2);
    RUN_TEST(fifo_valid_invalidReadPtr);
    RUN_TEST(fifo_valid_invalidReadPtr2);
    RUN_TEST(fifo_reset_resetsToBufferBegin);
    RUN_TEST(fifo_size_sameValueAsCreate);
    RUN_TEST(fifo_peek_single_peeksWithoutIncrementingReadPtr);
    RUN_TEST(fifo_peek_single_peeksFalseWhenNoData);
    RUN_TEST(fifo_peek_single_peekMultipleTimesEachTimeSame);
    RUN_TEST(fifo_peek_single_peekMultipleTimesReadMovingForward);
    RUN_TEST(fifo_has_next_item_returnsTrueWhenAvailable);
    RUN_TEST(fifo_has_next_item_returnsFalseWhenNotAvailable);
    RUN_TEST(fifo_has_next_item_returnsFalseWhenAlreadyRead);
    RUN_TEST(fifo_is_full_returnsFalseOnEmpty);
    RUN_TEST(fifo_is_full_smallFifoOverflow);
    RUN_TEST(fifo_is_full_smallFifoOverflow2);
    RUN_TEST(fifo_read_readsNothingWhenNoData);
    RUN_TEST(fifo_read_readsIntoBuffer);
    RUN_TEST(fifo_read_readNothingLimitedByArg);
    RUN_TEST(fifo_read_readLimitedByArg);
    RUN_TEST(fifo_read_readFull);
    RUN_TEST(fifo_peek_readsNothingWhenNoData);
    RUN_TEST(fifo_peek_readsIntoBuffer);
    RUN_TEST(fifo_peek_readNothingLimitedByArg);
    RUN_TEST(fifo_peek_readLimitedByArg);
    RUN_TEST(fifo_peek_readFull);
    RUN_TEST(fifo_write_noDataToWrite);
    RUN_TEST(fifo_write_writesAllData);
    RUN_TEST(fifo_write_smallBufferWriteUntilFull);
}
