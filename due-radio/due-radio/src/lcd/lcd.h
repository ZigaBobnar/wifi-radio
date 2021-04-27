#ifndef LIB_DRIVERS_LCD_H_
#define LIB_DRIVERS_LCD_H_

/**
* LCD driver for 16x2 character display
* It can display total of 32 ASCII characters, 16 per row.
* Those displays are based on Hitachi HD44780 chipset.
*
* Most common physical connections:
* PIN1 -> GND
* PIN2 -> VCC
* PIN3 -> Vo - contrast of the lcd
* PIN4 -> RS - Register select (0 - instruction register, 1 - data register)
* PIN5 -> R/W - Read or write to lcd (0 - read, 1 - write)
* PIN6 -> E - Enable the lcd chip
* PIN7-PIN14 -> D0-D7 - Data/command bus. Data can also be split in two parts and sent only through D4-D7, this reduces the pin usage but can be slower
* PIN15 -> (Optional) A - Anode for LED backlight - Positive terminal
* PIN16 -> (Optional) K - Cathode for LED backlight - Negative terminal
*/

#include "../common.h"

__EXTERN_C_BEGIN

#include <ioport.h>

/**
* HD44780U instruction set
*
* Instruction                            | RS | R/W | D7 - D0
* -----------------------------------------------------------------------------------------
* Clear display                             0    0    0000 0001
* Return home                               0    0    0000 001-
* Entry mode set                            0    0    0000 01(I/D)S
*   I/D=1 - increment
*   I/D=0 - decrement
*   S=1 - Accompanies display shift
* Display on/off control                    0    0    0000 1DCB
*   D=1 - sets display on
*   D=0 - sets display off
*   C=1 - sets cursor on
*   C=0 - sets cursor off
*   B=1 - enables cursor blinking
*   B=0 - disables cursor blinking
* Cursor or display shift                   0    0    0001 (S/C)(R/L)--
*   S/C=1 - display shift
*   S/C=0 - cursor move
*   R/L=1 - shift to the right
*   R/L=0 - shift to the left
* Function set                              0    0    001(DL) NF--
*   DL=1 - 8-bit data length mode
*   DL=0 - 4-bit data length mode
*   N=1 - 2 lines
*   N=0 - 1 line
*   F=1 - 5x10 dots character font
*   F=0 - 5x8 dots character font
* Set CGRAM address                         0    0    01(ACG)(ACG) (ACG)(ACG)(ACG)(ACG)
*   ACG - CGRAM address (character gen RAM)
* Set DDRAM address                         0    0    1(ADD)(ADD)(ADD) (ADD)(ADD)(ADD)(ADD)
*   ADD - DDRAM address (display data RAM)
* Read busy flag address                    0    1    (BF)(AC)(AC)(AC) (AC)(AC)(AC)(AC)
*   BF=1 - internally operating
*   BF=0 - instructions acceptable
*   AC - Address counter
* Write data to CG or DDRAM                 1    0    [Write data value]
* Read data from CG or DDRAM                1    1    [Read data value]
*/

enum lcd_command {
	clearDisplay         = 0x01,
	returnHome           = 0x02,
	entryModeSet         = 0x04,
	displayOnOff         = 0x08,
	cursorOrDisplayShift = 0x10,
	functionSet          = 0x20,
	setCGRAMAddress      = 0x40,
	setDDRAMAddress      = 0x80,
};

struct _lcd {
	/** R/S pin (0: command, 1: character) */
	uint32_t rs;
	/** R/W pin (0: write to LCD, 1: read from LCD) */
	uint32_t rw;
	/** Enable pin (1: enabled) */
	uint32_t enable;

	uint32_t d4;
	uint32_t d5;
	uint32_t d6;
	uint32_t d7;

	char __lcd_buffer[33];
	char* _lcd_string;
};
typedef struct _lcd lcd_t;


/**
*
* High level functions
*
*/

/**
* Initialize LCD driver
*/
void lcd_init(lcd_t* lcd);

/**
* Write contents of lcd_string to the display
*/
void lcd_write_lcd_string(lcd_t* lcd);

/**
* Write the value to the LCD as characters
* @param value 32 characters + 1 null terminator string
*/
void lcd_write_string(lcd_t* lcd, uint8_t* value);

/**
* Write string to the current position of LCD cursor
* @param value String to write to display
* @param length Number of characters of string to send to display
*/
void lcd_write_string_at_cursor(lcd_t* lcd, uint8_t* value, uint8_t length);

/**
* Blocking wait until the display has cleared busy status
*/
void lcd_wait_busy_status(lcd_t* lcd);


/**
*
* Low level LCD direct interfacing functions
*
*/

/**
* Sends the clear display command to the LCD
*/
void lcd_command_clear_display(lcd_t* lcd);

/**
* Sends the return home comand to the LCD
*/
void lcd_command_return_home(lcd_t* lcd);

/**
* Sends set entry mode command to the LCD
* @param increment Decrement or increment
* @param display_shift Accompanies display shift
*/
void lcd_command_entry_mode_set(lcd_t* lcd, bool increment, bool display_shift);

/**
* Sends the display on/off control command to the LCD
* @param display Set display on/off
* @param cursor Set cursor on/off
* @param blinking Enable/disable blinking
*/
void lcd_command_display_on_off(lcd_t* lcd, bool display, bool cursor, bool blinking);

/**
* Sends the cursor or display shift command to the LCD
* @param display_shift Display shift/cursor move
* @param shift_right Shift direction (right/left)
*/
void lcd_command_cursor_or_display_shift(lcd_t* lcd, bool display_shift, bool shift_right);

/**
* Sends the function set command to the LCD
* @param full_data_length Determines whether the display data bus is connected through 4 or 8 pins
* @param double_line Single/double line display
* @param full_height_character_font 5x8/5x10 dots character font
*/
void lcd_command_function_set(
lcd_t* lcd,
bool full_data_length,
bool double_line,
bool full_height_character_font);

/**
* Sends the set CGRAM address command to the LCD
* @param address 6-bit character generator RAM address
*/
void lcd_command_set_cgram_address(lcd_t* lcd, uint8_t address);

/**
* Sends the set DDRAM address to the LCD
* @param address 7-bit display data RAM address
*/
void lcd_command_set_ddram_address(lcd_t* lcd, uint8_t address);

/**
* Sends the read busy flag command to the LCD
* @returns Whether the display is busy or is ready to accept instructions
*/
bool lcd_command_read_busy_flag(lcd_t* lcd);

/**
* Sends the data or DDRAM value to the LCD
* @param value 8-bit value that will be sent to the display
*/
void lcd_driver_data_write(lcd_t* lcd, uint8_t value);

/**
* Reads the data or DDRAM value from the LCD
* @returns 8-bit data value
*/
uint8_t lcd_driver_data_read(lcd_t* lcd);

/**
* Sends the raw data and sets appropriate pins for command/data operations
* @param value The value to write to data pins
* @param is_command Determines whether the operation is command or data
*/
void lcd_driver_raw_send(lcd_t* lcd, uint8_t value, bool is_command);

/**
* Sets the lower 4 bits of the value on data pins
* @param value The value to write
*/
void lcd_driver_raw_data_pins_set(lcd_t* lcd, uint8_t value);

/**
* Sends a pulse (0, 1, 0) to the enable pin of LCD
*/
void lcd_driver_pulse_enable_pin(lcd_t* lcd);

__EXTERN_C_END

#endif  // LIB_DRIVERS_LCD_H_
