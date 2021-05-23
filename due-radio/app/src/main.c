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

        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
            "\n\nBoot setup has failed...\nCannot continue."
            CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        #if WIFI_RADIO_AUTOREBOOT == 1
            console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
                "Resetting in 10 seconds."
                CONSOLE_VT100_COLOR_TEXT_DEFAULT);

            delay_ms(10000);

            __DSB;
            SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
            RSTC->RSTC_CR = RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
            NVIC_SystemReset();
        #endif

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
    console_put_line(CONSOLE_VT100_COLOR_BG_BLACK
        CONSOLE_VT100_COLOR_TEXT_CYAN
        WIFI_RADIO_CONSOLE_BANNER
        CONSOLE_VT100_COLOR_BG_DEFAULT
        CONSOLE_VT100_COLOR_TEXT_DEFAULT);
    console_put_line("==> WiFi radio boot setup...");
    console_put_formatted(" => [LCD]\n (OK, %i ms)", (int)(lcd_end_time - lcd_start_time));
    console_put_line(" => [Console]");

    delay_ms(200);

    int32_t console_end_time = timeguard_get_time_ms();
    console_put_formatted(" (OK, %i ms)", (int)(console_end_time - console_start_time));


    /// Initialize DAC
    int32_t dac_start_time = timeguard_get_time_ms();
    lcd_write_lower_formatted(&lcd, "DAC...");
    console_put_line(" => [DAC]");

    dac_init(&dac);
    dac_write(&dac, 0);

    lcd_write_lower_formatted(&lcd, "DAC OK");

    delay_ms(200);

    int32_t dac_end_time = timeguard_get_time_ms();
    console_put_formatted(" (OK, %i ms)", (int)(dac_end_time - dac_start_time));


    /// Initialize ESP module
    int32_t esp_start_time = timeguard_get_time_ms();
    lcd_write_lower_formatted(&lcd, "ESP module...   ");
    console_put_line(" => [ESP module]");

    console_put_line("\tESP module hardware setup...");
    esp_module_hardware_setup(&lcd);

    delay_ms(100);

    console_put_line("\tESP module hardware setup OK");

    console_put_line("\tESP module init...");
    if (!esp_module_init()) {
        lcd_write_upper_formatted(&lcd, "ESP module init");
        lcd_write_lower_formatted(&lcd, " FAIL");

        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
            "\tESP module init FAIL" CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
            "\nFailed to initialize ESP module. This could be hardware problem."
            CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        return false;
    }

    console_put_line("\tESP module init OK");

    ////  Connect to WiFi
    lcd_write_lower_formatted(&lcd, "WiFi connect...");
    console_put_line("\tWiFi connect...");

    if (!esp_module_wifi_connect(WIFI_SSID, WIFI_PASSWORD)) {
        lcd_write_upper_formatted(&lcd, "WiFi connect");
        lcd_write_lower_formatted(&lcd, " FAIL");
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED "\tWiFi connect FAIL"
            CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
            "\nFailed to connect to WiFi." CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        return false;
    }

    console_put_line("\tWiFi connect OK");
    console_put_line("\t  WiFi connected to '" WIFI_SSID "'"); // Should SSID be queried from module?

    console_put_line("\tStarting audio stream...");
    esp_module_start_stream();
    console_put_line("\tStarting audio stream OK");

    lcd_write_lower_formatted(&lcd, "ESP module OK");

    delay_ms(200);

    int32_t esp_end_time = timeguard_get_time_ms();
    console_put_formatted(" (OK, %i ms)", (int)(esp_end_time - esp_start_time));


    lcd_write_upper_formatted(&lcd, "WiFi radio");
    lcd_write_lower_formatted(&lcd, " => Ready");
    lcd_write_lcd_string(&lcd);

    int32_t boot_end_time = timeguard_get_time_ms();
    console_put_formatted(CONSOLE_VT100_COLOR_TEXT_GREEN
        "Boot succeeded, took %i ms" CONSOLE_VT100_COLOR_TEXT_DEFAULT,
        (int)(boot_end_time - boot_start_time));
    console_put_line("WiFi radio ready.\n");

    return true;
}

void main_loop() {
    console_process_input();
    
    if (stream_running) {
        if (esp_module_rx_char_ready()) {
            esp_module_rx_read();

            if (dac_tx_ready(&dac)) {
                dac_write(&dac, (esp_rx_data*10));
            }
        }
    }
}

__EXTERN_C_END
