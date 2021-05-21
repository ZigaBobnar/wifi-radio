#include <asf.h>
#include "esp_module.h"

#ifdef __cplusplus
extern "C" {
#endif

int main (void)
{

    /* sets the processor clock according to conf_clock.h definitions */
    sysclk_init();

    /* disable wathcdog */
    WDT->WDT_MR = WDT_MR_WDDIS;

    /********************* HW init     ***************************/
    /*sysclk_enable_peripheral_clock(ID_PIOA);
    pio_set_peripheral(PIOA, PIO_PERIPH_A, PINS_UART);
    pio_pull_up(PIOA, PINS_UART, PIO_PULLUP);

    sysclk_enable_peripheral_clock(ID_UART);
    sam_uart_opt_t uart_opts = {
        .ul_mck = SystemCoreClock,
        .ul_baudrate = 115200,
        .ul_mode = UART_MR_CHMODE_NORMAL | UART_MR_PAR_NO,
    };
    uart_init(UART, &uart_opts);
    uart_reset_status(UART);
    uart_enable(UART);
    uart_enable_rx(UART);*/


    esp_module_hardware_setup();
    delay_ms(100);

    esp_module_init();
    esp_module_wifi_connect("test", "test1234");

    esp_module_start_stream();




    /********************* Main loop     ***************************/
    while(1)
    {
        if (esp_module_read()) {
            // TODO: Data recv
        }
    }

    while (1) {}
}

#ifdef __cplusplus
}
#endif
