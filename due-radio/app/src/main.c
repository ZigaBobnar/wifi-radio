#include <asf.h>
#include "esp_module.h"
#include "dac.h"
#include "lcd.h"
#include "console.h"

#ifdef __cplusplus
extern "C" {
#endif

lcd_t lcd = {
    .rs = PIO_PC12_IDX,     // Due pin 51, 	SAM3X8E pin PC12
    .rw	= PIO_PC14_IDX,     // Due pin 49,	SAM3X8E pin PC14
    .enable = PIO_PC16_IDX, // Due pin 47	SAM3X8E pin PC16

    .d4 = PIO_PC13_IDX,     // Due pin 50,	SAM3X8E pin PC13
    .d5 = PIO_PC15_IDX,     // Due pin 48,	SAM3X8E pin PC15
    .d6 = PIO_PC17_IDX,     // Due pin 46,	SAM3X8E pin PC17
    .d7 = PIO_PC19_IDX,     // Due pin 44,	SAM3X8E pin PC19

    .__lcd_buffer = "                                ",
};

console_t console = {
    .baud = CONSOLE_BAUD_115200,
};

dac_t dac = {
	.channel = 1,
	// .max_value = DACC_MAX_DATA,
	// .min_value = 100,
	// .sampling_frequency = 16000,
	// .get_next_sample_function = NULL,
};

int main (void)
{
    /* sets the processor clock according to conf_clock.h definitions */
    sysclk_init();

    /* disable wathcdog */
    WDT->WDT_MR = WDT_MR_WDDIS;

    /********************* HW init     ***************************/
    delay_init();
    ioport_init();

    /// Initialize LCD
    lcd_init(&lcd);

    lcd_clear_upper(&lcd);
    lcd_clear_lower(&lcd);
    sprintf(lcd.lcd_upper, "WiFi radio init");
    sprintf(lcd.lcd_lower, "LCD...");
    lcd_write_lcd_string(&lcd);

    delay_ms(100);

    /// Initialize Serial console
    sprintf(lcd.lcd_lower, "Console...");
    lcd_write_lcd_string(&lcd);

    console_init(&console);
    console_enable();

    console_put_string("WiFi radio init...\n");
    console_put_string(" => LCD OK\n");
    console_put_string(" => Console OK\n");

    console_put_string(" => DAC");
    dac_init(&dac);
    dac_write(&dac, 0);
    console_put_string(" OK\n");



/*
    esp_module_hardware_setup(&lcd);
    delay_ms(100);


    lcd_clear_lower(&lcd);
    sprintf(lcd.lcd_lower, "ESP init...");
    lcd_write_lcd_string(&lcd);

    if (esp_module_init()) {
        lcd_clear_lower(&lcd);
        sprintf(lcd.lcd_lower, "ESP connect...");
        lcd_write_lcd_string(&lcd);

        uint8_t connect_status = esp_module_wifi_connect("virus2\0\0", "susa2000\0\0");
        if (connect_status == 0x01) {
            lcd_clear_lower(&lcd);
            sprintf(lcd.lcd_lower, "Start stream...");
            lcd_write_lcd_string(&lcd);

            esp_module_start_stream();
        } else {
            lcd_clear_upper(&lcd);
            lcd_clear_lower(&lcd);
            sprintf(lcd.lcd_upper, "ESP conn fail...");
            sprintf(lcd.lcd_lower, "Code: %i", connect_status);
            lcd_write_lcd_string(&lcd);
        }
    } else {
        lcd_clear_lower(&lcd);
        sprintf(lcd.lcd_lower, "ESP init failed");
        lcd_write_lcd_string(&lcd);
    }
*/








    /********************* Main loop     ***************************/
    while(1)
    {
        /*if (esp_module_read()) {
            lcd_clear_lower(&lcd);
            sprintf(lcd.lcd_lower, "Recv: 0x%x", esp_rx_data);
            lcd_write_lcd_string(&lcd);
        }

        static bool dac1 = false;

        dac_write(&dac, dac1 * 500 + 200);
        dac1 = !dac1;*/
    }

    while (1) {}
}

#ifdef __cplusplus
}
#endif
