#include "drivers/lcd.h"
#include <delay.h>
#include <stdarg.h>
#include <memory.h>

__EXTERN_C_BEGIN

volatile lcd_t* g_lcd;

/**
 *
 * High level functions
 *
 */

void lcd_init(lcd_t* lcd) {
    g_lcd = lcd;

    ioport_init();

    ioport_set_pin_dir(g_lcd->d4, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d5, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d6, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d7, IOPORT_DIR_OUTPUT);

    ioport_set_pin_dir(g_lcd->enable, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->rs, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->rw, IOPORT_DIR_OUTPUT);

    ioport_set_pin_level(g_lcd->d4, 0);
    ioport_set_pin_level(g_lcd->d5, 0);
    ioport_set_pin_level(g_lcd->d6, 0);
    ioport_set_pin_level(g_lcd->d7, 0);

    ioport_set_pin_level(g_lcd->enable, 0);
    ioport_set_pin_level(g_lcd->rs, 0);
    ioport_set_pin_level(g_lcd->rw, 0);

    delay_us(50000);

    // We first write the function set command but only as D7-D4 bytes (other data pins are 0)
    // This will initialize the display for 4-bit operation
    lcd_driver_raw_data_pins_set((functionSet) >> 4);
    lcd_driver_pulse_enable_pin();
    delay_us(4000);
    lcd_wait_busy_status();

    // Now we can fully run function set command again.
    //lcd_command_function_set(lcd, false, true, false);
    lcd_command_function_set(
        /* full_data_length */ false,
        /* double_line */ true,
        /* full_height_character_font */ false);
    delay_us(100);
    lcd_wait_busy_status();

    /*ioport_set_pin_level(lcd->rs, 0);
    ioport_set_pin_level(lcd->rw, 0);
    lcd_driver_raw_send(lcd, 0x20|0x10, true);
    delay_us(4100);
    lcd_driver_raw_send(lcd, 0x20|0x10, true);
    delay_us(100);
    lcd_driver_raw_send(lcd, 0x20|0x10, true);
    delay_us(4000);
    lcd_driver_raw_send(lcd, 0x20|0x00, true);
    delay_us(100);
*/

    lcd_command_display_on_off(false, false, false);
    lcd_command_clear_display();
    lcd_command_cursor_or_display_shift(false, true);
    lcd_command_display_on_off(true, false, false);
}

void lcd_write_lcd_string() {
    lcd_write_string((uint8_t*)g_lcd->_lcd_string);
}

void lcd_write_string(uint8_t* value) {
    uint8_t* value_ptr = value;
    int i = 0;

    lcd_command_set_ddram_address(0x00);
    for (; i < 16; i++) {
        uint8_t char_value = *(value_ptr++);
        if (char_value == 0) {
            char_value = ' ';
        }

        lcd_driver_raw_send(char_value, false);
    }

    lcd_command_set_ddram_address(0x40);
    for (; i < 32; i++) {
        uint8_t char_value = *(value_ptr++);
        if (char_value == 0) {
            char_value = ' ';
        }

        lcd_driver_raw_send(char_value, false);
    }
}

void lcd_write_formatted(const char* format, ...) {
    va_list args;
    va_start(args, format);

    static char buffer[33];
    memset(buffer, 0, sizeof(buffer));

    vsnprintf(buffer, 33, format, args);
    memcpy(g_lcd->_lcd_string, buffer, 32 * sizeof(uint8_t));
    
    lcd_write_lcd_string();
}

void lcd_write_upper_formatted(const char* format, ...) {
    va_list args;
    va_start(args, format);

    static char buffer[17];
    memset(buffer, 0, sizeof(buffer));

    vsnprintf(buffer, 17, format, args);
    memcpy(g_lcd->lcd_upper, buffer, 16 * sizeof(uint8_t));

    lcd_write_lcd_string();
}

void lcd_write_lower_formatted(const char* format, ...) {
    va_list args;
    va_start(args, format);

    static char buffer[17];
    memset(buffer, 0, sizeof(buffer));

    vsnprintf(buffer, 17, format, args);
    memcpy(g_lcd->lcd_lower, buffer, 16 * sizeof(uint8_t));

    lcd_write_lcd_string();
}

void lcd_write_string_at_cursor(uint8_t* value, uint8_t length) {
    for (int i = 0; i < length; i++) {
        lcd_driver_raw_send(value[i], false);
    }
}

void lcd_wait_busy_status() {
    bool is_busy;

    ioport_set_pin_dir(g_lcd->d4, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d5, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d6, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d7, IOPORT_DIR_INPUT);

    ioport_set_pin_level(g_lcd->rs, 0);
    ioport_set_pin_level(g_lcd->rw, 1);

    do {
        delay_us(1);
        ioport_set_pin_level(g_lcd->enable, 1);
        delay_us(1);

        is_busy = ioport_get_pin_level(g_lcd->d7);
        ioport_set_pin_level(g_lcd->enable, 0);
        delay_us(1);

        ioport_set_pin_level(g_lcd->enable, 1);
        delay_us(1);

        ioport_set_pin_level(g_lcd->enable, 0);
    } while (is_busy);

    ioport_set_pin_dir(g_lcd->d4, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d5, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d6, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d7, IOPORT_DIR_OUTPUT);
}

void lcd_clear_upper() {
    sprintf(g_lcd->lcd_upper, "                ");
}

void lcd_clear_lower() {
    sprintf(g_lcd->lcd_lower, "                ");
}


/**
 *
 * Low level LCD direct interfacing functions
 *
 */

void lcd_command_clear_display() {
    lcd_driver_raw_send(clearDisplay, true);
    delay_us(2000);
}

void lcd_command_return_home() {
    lcd_driver_raw_send(returnHome, true);
    delay_us(2000);
}

void lcd_command_entry_mode_set(bool increment, bool display_shift) {
    lcd_driver_raw_send(entryModeSet | (increment << 1) | (display_shift << 0), true);
}

void lcd_command_display_on_off(bool display, bool cursor, bool blinking) {
    lcd_driver_raw_send(displayOnOff | (display << 2) | (cursor << 1) | (blinking << 0), true);
}

void lcd_command_cursor_or_display_shift(bool display_shift, bool shift_right) {
    lcd_driver_raw_send(cursorOrDisplayShift | (display_shift << 3) | (shift_right << 2), true);
}

void lcd_command_function_set(bool full_data_length, bool double_line, bool full_height_character_font) {
    lcd_driver_raw_send(functionSet | (full_data_length << 4) | (double_line << 3) | (full_height_character_font << 2), true);
}

void lcd_command_set_cgram_address(uint8_t address) {
    lcd_driver_raw_send(setCGRAMAddress | (0x3F & address), true);
}

void lcd_command_set_ddram_address(uint8_t address) {
    lcd_driver_raw_send(setDDRAMAddress | (0x7F & address), true);
}

bool lcd_command_read_busy_flag() {
    ioport_set_pin_dir(g_lcd->d4, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d5, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d6, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d7, IOPORT_DIR_INPUT);

    ioport_set_pin_level(g_lcd->rs, 0);
    ioport_set_pin_level(g_lcd->rw, 1);

    ioport_set_pin_level(g_lcd->enable, 0);
    delay_us(1);

    ioport_set_pin_level(g_lcd->enable, 1);
    bool busy_flag = ioport_get_pin_level(g_lcd->d7);
    delay_us(1);

    ioport_set_pin_level(g_lcd->enable, 0);
    delay_us(1);

    ioport_set_pin_dir(g_lcd->d4, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d5, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d6, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d7, IOPORT_DIR_OUTPUT);

    return busy_flag;
}

void lcd_driver_data_write(uint8_t value) {
    lcd_driver_raw_send(value, false);
}

uint8_t lcd_driver_data_read() {
    ioport_set_pin_dir(g_lcd->d4, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d5, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d6, IOPORT_DIR_INPUT);
    ioport_set_pin_dir(g_lcd->d7, IOPORT_DIR_INPUT);

    ioport_set_pin_level(g_lcd->rs, 0);
    ioport_set_pin_level(g_lcd->rw, 1);

    ioport_set_pin_level(g_lcd->enable, 0);
    delay_us(1);

    ioport_set_pin_level(g_lcd->enable, 1);
    bool busy_flag = ioport_get_pin_level(g_lcd->d7);
    delay_us(1);

    ioport_set_pin_level(g_lcd->enable, 0);
    delay_us(1);

    ioport_set_pin_dir(g_lcd->d4, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d5, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d6, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(g_lcd->d7, IOPORT_DIR_OUTPUT);

    return busy_flag;
}

void lcd_driver_raw_send(uint8_t value, bool is_command) {
    ioport_set_pin_level(g_lcd->rs, !is_command);
    ioport_set_pin_level(g_lcd->rw, 0);

    // ioport_set_pin_level(lcd->d7, value & (1 << 7));
    // ioport_set_pin_level(lcd->d6, value & (1 << 6));
    // ioport_set_pin_level(lcd->d5, value & (1 << 5));
    // ioport_set_pin_level(lcd->d4, value & (1 << 4));

    lcd_driver_raw_data_pins_set(value >> 4);

    lcd_driver_pulse_enable_pin();


    lcd_driver_raw_data_pins_set(value);

    // ioport_set_pin_level(lcd->d7, value & (1 << 3));
    // ioport_set_pin_level(lcd->d6, value & (1 << 2));
    // ioport_set_pin_level(lcd->d5, value & (1 << 1));
    // ioport_set_pin_level(lcd->d4, value & (1 << 0));

    lcd_driver_pulse_enable_pin();
}

void lcd_driver_raw_data_pins_set(uint8_t value) {
    ioport_set_pin_level(g_lcd->d7, value & (1 << 3));
    ioport_set_pin_level(g_lcd->d6, value & (1 << 2));
    ioport_set_pin_level(g_lcd->d5, value & (1 << 1));
    ioport_set_pin_level(g_lcd->d4, value & (1 << 0));
}

void lcd_driver_pulse_enable_pin() {
    ioport_set_pin_level(g_lcd->enable, 0);
    delay_us(1);

    ioport_set_pin_level(g_lcd->enable, 1);
    delay_us(1);

    ioport_set_pin_level(g_lcd->enable, 0);
    delay_us(100);
}


__EXTERN_C_END
