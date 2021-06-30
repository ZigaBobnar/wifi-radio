#include "app/setup.h"
#include "app/runtime.h"
#include "drivers/lcd.h"
#include "drivers/dac.h"
#include "drivers/esp_module.h"
#include "drivers/console.h"
#include "utils/timeguard.h"
#include "app/ui.h"
#include "app/audio_player.h"

__EXTERN_C_BEGIN

bool setup_begin(void) {
    int32_t boot_start_time = timeguard_get_time_ms();

    if (!setup_hardware())
        return false;
    int32_t hardware_time = timeguard_get_time_ms();

    if (!setup_lcd())
        return false;
    int32_t lcd_time = timeguard_get_time_ms();

    if (!setup_console())
        return false;
    int32_t console_time = timeguard_get_time_ms();
    console_put_formatted(" => [LCD]\n (OK, %i ms)", (int)(lcd_time - hardware_time));
    console_put_formatted(" => [Console]\n (OK, %i ms)", (int)(console_time - lcd_time));
 
    if (!setup_dac())
        return false;
    int32_t dac_time = timeguard_get_time_ms();
    console_put_formatted(" => [DAC]\n (OK, %i ms)", (int)(console_time - lcd_time));

    if (!setup_esp_module())
        return false;
    int32_t esp_time = timeguard_get_time_ms();

    if (!setup_esp_wifi())
        return false;
    int32_t wifi_time = timeguard_get_time_ms();

    if (!setup_timesync())
        return false;
    int32_t timesync_time = timeguard_get_time_ms();

    if (!setup_player())
        return false;
    int32_t player_time = timeguard_get_time_ms();

    lcd_write_lower_formatted("ESP module OK");

    console_put_formatted(" (OK, %i ms)", (int)(timesync_time - dac_time));

    lcd_write_upper_formatted("WiFi radio");
    lcd_write_lower_formatted(" => Ready");
    lcd_write_lcd_string();

    console_put_formatted(CONSOLE_VT100_COLOR_TEXT_GREEN
        "Boot succeeded, took %i ms" CONSOLE_VT100_COLOR_TEXT_DEFAULT,
        (int)(timeguard_get_diff_ms(boot_start_time)));
    console_put_line("WiFi radio ready.\n");

    return true;
}

bool setup_hardware(void) {
    delay_init();
    ioport_init();
    timeguard_init();
    ui_init();

    return true;
}

bool setup_lcd(void) {
    lcd_init(runtime->lcd);

    lcd_clear_upper();
    lcd_clear_lower();

    lcd_write_upper_formatted("WiFi radio init");
    lcd_write_lower_formatted("LCD OK");

    delay_ms(100);

    return true;
}

bool setup_console() {
    lcd_write_lower_formatted("Console...");

    console_init();
    console_enable();

    lcd_write_lower_formatted("Console OK");
    console_put_line(CONSOLE_VT100_COLOR_BG_BLACK
        CONSOLE_VT100_COLOR_TEXT_CYAN
        WIFI_RADIO_CONSOLE_BANNER
        CONSOLE_VT100_COLOR_BG_DEFAULT
        CONSOLE_VT100_COLOR_TEXT_DEFAULT);
    console_put_line("==> WiFi radio boot setup...");

    delay_ms(100);

    return true;
}

bool setup_dac(void) {
    lcd_write_lower_formatted("DAC...");
    console_put_line(" => [DAC]");

    dac_init(runtime->dac);
    dac_write(0);

    lcd_write_lower_formatted("DAC OK");

    delay_ms(100);
    
    return true;
}

bool setup_esp_module(void) {
    lcd_write_lower_formatted("ESP module...   ");
    console_put_line(" => [ESP module]");

    console_put_line("\tESP module hardware setup...");
    esp_module_hardware_setup();

    delay_ms(100);

    console_put_line("\tESP module hardware setup OK");

    console_put_line("\tESP module init...");
    if (!esp_module_init()) {
        lcd_write_upper_formatted("ESP module init");
        lcd_write_lower_formatted(" FAIL");

        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
                         "\tESP module init FAIL" CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
                         "\nFailed to initialize ESP module. This could be hardware problem." CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        return false;
    }

    console_put_line("\tESP module init OK");

    return true;
}

bool setup_esp_wifi(void) {
    lcd_write_lower_formatted("WiFi connect...");
    console_put_line("\tWiFi connect...");

    if (!esp_module_wifi_connect(WIFI_SSID, WIFI_PASSWORD)) {
        lcd_write_upper_formatted("WiFi connect");
        lcd_write_lower_formatted(" FAIL");
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED "\tWiFi connect FAIL" CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
                         "\nFailed to connect to WiFi." CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        return false;
    }

    console_put_line("\tWiFi connect OK");
    console_put_line("\t  WiFi connected to '" WIFI_SSID "'"); // Should SSID be queried from module?

    return true;
}

bool setup_timesync(void) {
    console_put_line("\tSyncing time...");

    current_time *c_time = esp_module_get_current_time();
    if (c_time == NULL) {
        lcd_write_upper_formatted("Time sync");
        lcd_write_lower_formatted(" FAIL");
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED "\tTime sync FAIL" CONSOLE_VT100_COLOR_TEXT_DEFAULT);
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
                         "\nFailed to sync with the current time." CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        return false;
    }
    else
    {
        ui_update_current_time();
        ui_set_current_time_ms((c_time->hour * 60 * 60 + c_time->minutes * 60 + c_time->seconds) * 1000);        
        ui_set_current_date(c_time->year, c_time->month, c_time->day);

        console_put_formatted("\t  Time synced, %i-%i-%i %i:%i:%i", c_time->year, c_time->month, c_time->day, c_time->hour, c_time->minutes, c_time->seconds);
        console_put_formatted("\t  UI time: %i ms", ui_get_current_time_ms());

        free(c_time);
    }

    console_put_line("\tTime sync OK");

    return true;
}

bool setup_player(void) {
    audio_player_init();

    return true;
}

__EXTERN_C_END
