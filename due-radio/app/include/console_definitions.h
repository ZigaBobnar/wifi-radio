#ifndef CONSOLE_DEFINITIONS_H_
#define CONSOLE_DEFINITIONS_H_

/**
 * Console definitions
 * 
 * This file serves as a collection of definitions and macros that can be used
 * for easier console configuration and operation or anywhere else where it
 * fits. It is a separate file so it can be included to other modules and so it
 * does not spam the console interface header.
 */


#define CONSOLE_BAUD_4800       (4800)
#define CONSOLE_BAUD_9600       (9600)
#define CONSOLE_BAUD_14400      (14400)
#define CONSOLE_BAUD_19200      (19200)
#define CONSOLE_BAUD_38400      (38400)
#define CONSOLE_BAUD_57600      (57600)
#define CONSOLE_BAUD_74880      (74880)
#define CONSOLE_BAUD_115200     (115200)


#define CONSOLE_ASCII_NULL              0x00 /* '\0' */
#define CONSOLE_ASCII_BELL              0x07 /* '\a' */
#define CONSOLE_ASCII_BACKSPACE         0x08 /* '\b' */
#define CONSOLE_ASCII_HORIZONTAL_TAB    0x09 /* '\t' */
#define CONSOLE_ASCII_LINEFEED          0x0A /* '\n' */
#define CONSOLE_ASCII_VERTICAL_TAB      0x0B /* '\v' */
#define CONSOLE_ASCII_FORMFEED          0x0C /* '\f' */
#define CONSOLE_ASCII_CARRIAGE_RETURN   0x0D /* '\r' */
#define CONSOLE_ASCII_ESCAPE            0x1B /* '\033' */
#define CONSOLE_ASCII_DELETE            0x7F



/**
 * VT100 terminal codes use escape sequences to send commands such as colors or
 * cursor positioning to terminal.
 * 
 * The definitions can be found on the internet, I have mainly used those:
 * - https://domoticx.com/terminal-codes-ansivt100/
 * - https://espterm.github.io/docs/VT100%20escape%20codes.html
 * - https://vt100.net/docs/vt100-ug/chapter3.html
 */

// Cursor positioning

#define CONSOLE_VT100_SET_CURSOR_POS_0_0            "\033[0;0H"
#define CONSOLE_VT100_SET_CURSOR_POS_x_y(x, y)      "\033[" #x ";" #y "H"
#define CONSOLE_VT100_CURSOR_UP                     "\033[A"
#define CONSOLE_VT100_CURSOR_DOWN                   "\033[B"
#define CONSOLE_VT100_CURSOR_RIGHT                  "\033[C"
#define CONSOLE_VT100_CURSOR_LEFT                   "\033[D"
#define CONSOLE_VT100_CURSOR_UP_n(n)                "\033[" #n "A"
#define CONSOLE_VT100_CURSOR_DOWN_n(n)              "\033[" #n "B"
#define CONSOLE_VT100_CURSOR_RIGHT_n(n)             "\033[" #n "C"
#define CONSOLE_VT100_CURSOR_LEFT_n(n)              "\033[" #n "D"

// Erasing
// Erasing is inclusive to the character it represents
// EOL=End of line, BOL=Beginning of line

#define CONSOLE_VT100_ERASE_TO_EOL                  "\033[K"    /* can also be 'ESC [ 0 K' */
#define CONSOLE_VT100_ERASE_TO_BOL                  "\033[1K"
#define CONSOLE_VT100_ERASE_LINE                    "\033[2K"   /* cursor position does not change! */
#define CONSOLE_VT100_ERASE_TO_END_OF_SCREEN        "\033[J"    /* can also be 'ESC [ 0 J' */
#define CONSOLE_VT100_ERASE_TO_BEGINNING_OF_SCREEN  "\033[1J"
#define CONSOLE_VT100_ERASE_SCREEN                  "\033[2J"

// Foreground coloring

#define CONSOLE_VT100_COLOR_TEXT_BLACK              "\033[30m"
#define CONSOLE_VT100_COLOR_TEXT_RED                "\033[31m"
#define CONSOLE_VT100_COLOR_TEXT_GREEN              "\033[32m"
#define CONSOLE_VT100_COLOR_TEXT_YELLOW             "\033[33m"
#define CONSOLE_VT100_COLOR_TEXT_BLUE               "\033[34m"
#define CONSOLE_VT100_COLOR_TEXT_MAGENTA            "\033[35m"
#define CONSOLE_VT100_COLOR_TEXT_CYAN               "\033[36m"
#define CONSOLE_VT100_COLOR_TEXT_WHITE              "\033[37m"
#define CONSOLE_VT100_COLOR_TEXT_DEFAULT            "\033[39m"

// Background coloring

#define CONSOLE_VT100_COLOR_BG_BLACK                "\033[40m"
#define CONSOLE_VT100_COLOR_BG_RED                  "\033[41m"
#define CONSOLE_VT100_COLOR_BG_GREEN                "\033[42m"
#define CONSOLE_VT100_COLOR_BG_YELLOW               "\033[43m"
#define CONSOLE_VT100_COLOR_BG_BLUE                 "\033[44m"
#define CONSOLE_VT100_COLOR_BG_MAGENTA              "\033[45m"
#define CONSOLE_VT100_COLOR_BG_CYAN                 "\033[46m"
#define CONSOLE_VT100_COLOR_BG_WHITE                "\033[47m"
#define CONSOLE_VT100_COLOR_BG_DEFAULT              "\033[49m"

// Font (text) size
// DW=Double width, SW=Single width
// DH=Double height, SH=Single height

#define CONSOLE_VT100_TEXT_DWDH                     "\033[#5"
#define CONSOLE_VT100_TEXT_SWSH                     "\033[#6"


#endif  // CONSOLE_DEFINITIONS_H_
