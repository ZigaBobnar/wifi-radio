#include "common.h"
#include "esp_module.h"
#include "dac.h"
#include "lcd.h"
#include "console.h"
#include "timeguard.h"

__EXTERN_C_BEGIN

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

dac_t dac = {
	.channel = 1,
	// .max_value = DACC_MAX_DATA,
	// .min_value = 100,
	// .sampling_frequency = 16000,
	// .get_next_sample_function = NULL,
};

bool boot_setup(void);
void main_loop(void);

int main (void)
{
    /* sets the processor clock according to conf_clock.h definitions */
    sysclk_init();

    /* disable wathcdog */
    WDT->WDT_MR = WDT_MR_WDDIS;

    /*****************************   HW init     *****************************/
    bool setup_successful = boot_setup();
    if (setup_successful) {
        // Initialization has succeeded.
        /***************************   Main loop   ***************************/

        while (1) {
            main_loop();
        }
    } else {
        // Boot has failed, abort.
        delay_ms(3000);
        lcd_write_upper_formatted(&lcd, "Boot setup fail");
        lcd_write_lower_formatted(&lcd, "Cannot continue");

        console_put_string(CONSOLE_VT100_COLOR_TEXT_RED
            "\n\nBoot setup has failed...\nCannot continue.\n"
            CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        while (1) {}
    }

    while (1) {}
}

bool boot_setup() {
    delay_init();
    ioport_init();
    timeguard_init();
    int32_t boot_start_time = timeguard_get_time_ms();

    /// Initialize LCD
    int32_t lcd_start_time = timeguard_get_time_ms();
    lcd_init(&lcd);

    lcd_clear_upper(&lcd);
    lcd_clear_lower(&lcd);

    int32_t lcd_finish_time = timeguard_get_time_ms();
    lcd_write_upper_formatted(&lcd, "WiFi radio init");
    lcd_write_lower_formatted(&lcd, "LCD OK");

    delay_ms(200);
    int32_t lcd_end_time = timeguard_get_time_ms();


    /// Initialize Serial console
    int32_t console_start_time = timeguard_get_time_ms();
    lcd_write_lower_formatted(&lcd, "Console...");

    console_init();
    console_enable();

    lcd_write_lower_formatted(&lcd, "Console OK");
    console_put_string(CONSOLE_VT100_COLOR_BG_BLACK
        CONSOLE_VT100_COLOR_TEXT_CYAN
        WIFI_RADIO_CONSOLE_BANNER
        CONSOLE_VT100_COLOR_BG_DEFAULT
        CONSOLE_VT100_COLOR_TEXT_DEFAULT);
    console_put_string("==> WiFi radio boot setup...\n");
    console_put_formatted(" => [LCD]\n (OK, %i ms)\n", (int)(lcd_end_time - lcd_start_time));
    console_put_string(" => [Console]\n");

    delay_ms(200);

    int32_t console_end_time = timeguard_get_time_ms();
    console_put_formatted(" (OK, %i ms)\n", (int)(console_end_time - console_start_time));


    /// Initialize DAC
    int32_t dac_start_time = timeguard_get_time_ms();
    lcd_write_lower_formatted(&lcd, "DAC...");
    console_put_string(" => [DAC]\n");

    dac_init(&dac);
    dac_write(&dac, 0);
    
    lcd_write_lower_formatted(&lcd, "DAC OK");

    delay_ms(200);

    int32_t dac_end_time = timeguard_get_time_ms();
    console_put_formatted(" (OK, %i ms)\n", (int)(dac_end_time - dac_start_time));


    /// Initialize ESP module
    int32_t esp_start_time = timeguard_get_time_ms();
    lcd_write_lower_formatted(&lcd, "ESP module...   ");
    console_put_string(" => [ESP module]\n");

    console_put_string("\tESP module hardware setup...\n");
    esp_module_hardware_setup(&lcd);
    
    delay_ms(100);

    console_put_string("\tESP module hardware setup OK\n");

    console_put_string("\tESP module init...\n");
    if (!esp_module_init()) {
        lcd_write_upper_formatted(&lcd, "ESP module init");
        lcd_write_lower_formatted(&lcd, " FAIL");
        
        console_put_string(CONSOLE_VT100_COLOR_TEXT_RED
            "\tESP module init FAIL\n" CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        console_put_string(CONSOLE_VT100_COLOR_TEXT_RED
            "\nFailed to initialize ESP module. This could be hardware problem.\n"
            CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        
        return false;
    }

    console_put_string("\tESP module init OK\n");

    ////  Connect to WiFi
    lcd_write_lower_formatted(&lcd, "WiFi connect...");
    console_put_string("\tWiFi connect...\n");

    if (!esp_module_wifi_connect(WIFI_SSID, WIFI_PASSWORD)) {
        lcd_write_upper_formatted(&lcd, "WiFi connect");
        lcd_write_lower_formatted(&lcd, " FAIL");
        console_put_string(CONSOLE_VT100_COLOR_TEXT_RED "\tWiFi connect FAIL\n"
            CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        console_put_string(CONSOLE_VT100_COLOR_TEXT_RED
            "\nFailed to connect to WiFi.\n" CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        
        return false;
    }

    console_put_string("\tWiFi connect OK\n");
    console_put_string("\t  WiFi connected to " WIFI_SSID "\n"); // Should SSID be queried from module?

    console_put_string("\tStarting audio stream...\n");
    esp_module_start_stream();
    console_put_string("\tStarting audio stream OK\n");
    
    lcd_write_lower_formatted(&lcd, "ESP module OK");

    delay_ms(200);
    
    int32_t esp_end_time = timeguard_get_time_ms();
    console_put_formatted(" (OK, %i ms)\n", (int)(esp_end_time - esp_start_time));


    lcd_write_upper_formatted(&lcd, "WiFi radio");
    lcd_write_lower_formatted(&lcd, " => Ready");
    lcd_write_lcd_string(&lcd);

    int32_t boot_end_time = timeguard_get_time_ms();
    console_put_formatted(CONSOLE_VT100_COLOR_TEXT_GREEN
        "Boot succeeded, took %i ms\n" CONSOLE_VT100_COLOR_TEXT_DEFAULT,
        (int)(boot_end_time - boot_start_time));
    console_put_string("WiFi radio ready.\n\n");

    return true;
}

void main_loop() {
    /*if (esp_module_read()) {
        lcd_clear_lower(&lcd);
        sprintf(lcd.lcd_lower, "Recv: 0x%x", esp_rx_data);
        lcd_write_lcd_string(&lcd);
    }

    static bool dac1 = false;

    dac_write(&dac, dac1 * 500 + 200);
    dac1 = !dac1;*/
}

__EXTERN_C_END
