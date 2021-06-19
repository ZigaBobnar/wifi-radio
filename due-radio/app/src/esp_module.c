#include "esp_module.h"
#include <string.h>
#include <stdarg.h>
#include "console.h"
#include "timeguard.h"
#include "audio_player.h"

__EXTERN_C_BEGIN

uint8_t esp_rx_data = 0;

uint8_t esp_rx_fifo_buff[ESP_RX_QUEUE_SIZE];
fifo_t esp_rx_fifo = {
    .read_idx = 0,
    .write_idx = 0,
    .size = ESP_RX_QUEUE_SIZE,
    .buffer = esp_rx_fifo_buff,
};

bool reading_raw_chunk = false;

void esp_module_hardware_setup()
{
    pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA10A_RXD0 | PIO_PA11A_TXD0);
    pio_pull_up(PIOA, PIO_PA10A_RXD0 | PIO_PA11A_TXD0, PIO_PULLUP);

    sysclk_enable_peripheral_clock(ID_USART0);
    sam_usart_opt_t usart_opts = {
        .baudrate = /*74880*/ /*115200*/ 500000,
        .char_length = US_MR_CHRL_8_BIT,
        .parity_type = US_MR_PAR_NO,
        .stop_bits = US_MR_NBSTOP_1_BIT,
    };
    usart_init_rs232(USART0, &usart_opts, sysclk_get_peripheral_hz());

    usart_enable_tx(USART0);
    usart_enable_rx(USART0);

    USART0->US_IER = US_IER_RXRDY;
    NVIC_EnableIRQ(USART0_IRQn);
}

bool esp_module_init()
{
    int8_t retries_left = 10;

    while (retries_left >= 0)
    {
        console_put_formatted("ESP> Sending module init command, tries left: %i", retries_left);

        esp_module_clear_status();

        esp_module_tx_put_line("status");

        char *response = esp_module_rx_read_line(31, 100);
        if (response != NULL)
        {
            if (strcmp(response, "OK") == 0)
            {
                console_put_line("ESP> Module initialized.");
                free(response);

                return true;
            }

            console_put_line("ESP> Module init failed. Incorrect response.");
            free(response);
        }

        retries_left--;

        delay_ms(500);
    }

    return false;
}

bool esp_module_wifi_connect(const char *ssid, const char *password)
{
    console_put_line("ESP> Sending wifi connect command");

    esp_module_clear_status();

    esp_module_tx_put_formatted("connect %s %s", ssid, password);
    delay_ms(100);

    char *response = esp_module_rx_read_line(127, 15000);
    if (response != NULL)
    {
        if (strcmp(response, "OK") == 0)
        {
            console_put_line("ESP> Module wifi connected.");
            free(response);

            return true;
        }

        console_put_line("ESP> Module wifi connect failed. Incorrect response.");
        free(response);

        return false;
    }
}

void esp_module_play_next()
{
    console_put_line("ESP> Sending play next command");

    esp_module_clear_status();
    esp_module_tx_put_line("play_next");
    delay_ms(10);
    esp_module_clear_status();
}

void esp_module_play_previous()
{
    console_put_line("ESP> Sending play previous command");

    esp_module_clear_status();
    esp_module_tx_put_line("play_previous");
    delay_ms(10);
    esp_module_clear_status();
}

currently_playing_info *esp_module_get_currently_playing()
{
    console_put_line("ESP> Querying currently playing track");

    esp_module_clear_status();
    esp_module_tx_put_line("get_currently_playing");

    char *response = esp_module_rx_read_line(127, 2000);
    if (response != NULL)
    {
        if (strlen(response) > 4 && response[0] == 'O' && response[1] == 'K' && response[2] == ' ')
        {
            console_put_line("ESP> Received currently playing track information.");

            if (!strlen(response) > 11 || response[3] != 'P' || response[4] != 'L' || response[5] != 'A' || response[6] != 'Y' || response[7] != 'I' || response[8] != 'N' || response[9] != 'G' || response[10] != ' ')
            {
                console_put_line("ESP> Track is stopped.");
                /*"OK STOPPED"*/
                free(response);

                return NULL;
            }
            else
            {
                char track_length_ms_str[8] = {0, 0, 0, 0, 0, 0, 0, 0},
                     sampling_frequency_str[8] = {0, 0, 0, 0, 0, 0, 0, 0},
                     current_chunk_str[8] = {0, 0, 0, 0, 0, 0, 0, 0},
                     total_chunks_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int read_pos = 0;
                int write_index = 0;

                for (int i = 11; i < strlen(response); i++)
                {
                    char c = response[i];
                    if (c == ';')
                    {
                        read_pos++;
                        write_index = 0;
                    }
                    else if (c == 0)
                    {
                        break;
                    }
                    else
                    {
                        switch (read_pos)
                        {
                        case 0:
                            track_length_ms_str[write_index] = c;
                            break;
                        case 1:
                            sampling_frequency_str[write_index] = c;
                            break;
                        case 2:
                            current_chunk_str[write_index] = c;
                            break;
                        case 3:
                            total_chunks_str[write_index] = c;
                            break;
                        }

                        write_index++;
                    }
                }

                currently_playing_info *result = calloc(1, sizeof(currently_playing_info));
                result->track.track_length_ms = atoi(track_length_ms_str);
                result->track.sampling_frequency = atoi(sampling_frequency_str);
                result->current_chunk = atoi(current_chunk_str);
                result->track.total_chunks = atoi(total_chunks_str);

                free(response);

                return result;
            }
        }

        console_put_formatted("ESP> Querying currently playing track failed. Incorrect response %s.", response);
        free(response);

        return NULL;
    }
}

track_info *esp_module_get_track_info(int track_id)
{
    console_put_formatted("ESP> Querying track id: %i", track_id);

    esp_module_clear_status();
    esp_module_tx_put_formatted("get_track_info %i", track_id);

    char *response = esp_module_rx_read_line(127, 2000);
    if (response != NULL)
    {
        if (strlen(response) > 4 && response[0] == 'O' && response[1] == 'K' && response[2] == ' ')
        {
            console_put_formatted("ESP> Received track %i information.", track_id);

            char track_length_ms_str[8] = {0, 0, 0, 0, 0, 0, 0, 0},
                 sampling_frequency_str[8] = {0, 0, 0, 0, 0, 0, 0, 0},
                 total_chunks_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            int read_pos = 0;
            int write_index = 0;

            for (int i = 3; i < strlen(response); i++)
            {
                char c = response[i];
                if (c == ';')
                {
                    read_pos++;
                    write_index = 0;
                }
                else if (c == 0)
                {
                    break;
                }
                else
                {
                    switch (read_pos)
                    {
                    case 0:
                        track_length_ms_str[write_index] = c;
                        break;
                    case 1:
                        sampling_frequency_str[write_index] = c;
                        break;
                    case 2:
                        total_chunks_str[write_index] = c;
                        break;
                    }

                    write_index++;
                }
            }

            track_info *result = calloc(1, sizeof(track_info));
            result->track_length_ms = atoi(track_length_ms_str);
            result->sampling_frequency = atoi(sampling_frequency_str);
            result->total_chunks = atoi(total_chunks_str);

            free(response);

            return result;
        }
        else
        {
            console_put_formatted("ESP> Querying track info failed. Incorrect response %s.", response);
            free(response);

            return NULL;
        }
    }

    return NULL;
}

int esp_module_get_chunk(int track_id, int chunk_index)
{
    console_put_formatted("ESP> Retrieving chunk %i for track id: %i", chunk_index, track_id);

    audio_player_buffering = true;

    esp_module_clear_status();
    esp_module_tx_put_formatted("get_chunk %i %i", track_id, chunk_index);

    /*char *response = esp_module_rx_read_line(127, 2000);
    if (response != NULL)
    {
        if (strlen(response) > 4 && response[0] == 'O' && response[1] == 'K' && response[2] == ' ')
        {
            console_put_formatted("ESP> Received chunk %i information.", track_id);

            char chunk_length_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            int write_index = 0;

            for (int i = 3; i < strlen(response); i++)
            {
                char c = response[i];
                if (c == 0)
                {
                    break;
                }
                else
                {
                    chunk_length_str[write_index] = c;

                    write_index++;
                }
            }

            int current_chunk_length = atoi(chunk_length_str);
            audio_player_buffering_samples_left = current_chunk_length;

            free(response);

            for (int n = 0; n < current_chunk_length; n++) {
                if (esp_module_rx_read()) {
                    fifo_write_single(audio_player_buffer, esp_rx_data);
                    audio_player_buffering_samples_left--;
                } else {
                    console_put_formatted("ESP> Read %i chunks from queue.", n + 1);
                    break;
                }
            }

            return current_chunk_length;
        }
        else
        {
            console_put_formatted("ESP> Retrieving chunk failed. Incorrect response %s.", response);
            free(response);

            return 0;
        }
    }*/

    return 0;
}

current_time *esp_module_get_current_time()
{
    console_put_line("ESP> Querying current time");

    esp_module_clear_status();
    esp_module_tx_put_line("get_current_time");

    char *response = esp_module_rx_read_line(31, 2000);
    if (response != NULL)
    {
        if (strlen(response) != 22 || response[0] != 'O' || response[1] != 'K' || response[2] != ' ')
        {
            console_put_formatted("ESP> Querying current time failed. Incorrect response %s.", response);
            free(response);

            return NULL;
        }

        console_put_formatted("ESP> Received current time information. %s", response);

        char year[5] = {
            response[3], response[4], response[5], response[6]},
             month[3] = {response[8], response[9]}, day[3] = {response[11], response[12]}, hour[3] = {response[14], response[15]}, minutes[3] = {response[17], response[18]}, seconds[3] = {response[20], response[21]};

        current_time *result = calloc(1, sizeof(current_time));
        result->year = atoi(year);
        result->month = atoi(month);
        result->day = atoi(day);
        result->hour = atoi(hour);
        result->minutes = atoi(minutes);
        result->seconds = atoi(seconds);

        free(response);

        return result;
    }
}

/*void esp_module_start_stream() {
    console_put_line("ESP> Sending start stream command");

    esp_module_clear_status();
    esp_module_tx_put_line("start_stream");
    stream_running = true;

    delay_ms(100);
    esp_module_clear_status();
}*/

/*void esp_module_stop_stream() {
    console_put_line("ESP> Sending stop stream command");

    esp_module_clear_status();
    esp_module_tx_put_line("stop_stream");
    stream_running = false;

    delay_ms(100);
    esp_module_clear_status();
}*/

void esp_module_tx_put_char(uint8_t value)
{
    while (!(USART0->US_CSR & US_CSR_TXRDY))
    {
    }

    USART0->US_THR = US_THR_TXCHR(value);
}

void esp_module_tx_put_line(const char *value)
{
    console_put_formatted("ESP(TX)> %s", value);

    for (int i = 0; i < strlen(value); i++)
    {
        esp_module_tx_put_char(value[i]);
    }
    esp_module_tx_put_char('\n');
}

void esp_module_tx_put_formatted(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    size_t required_size = vsnprintf(NULL, 0, format, args) + 1;

    char *buffer = calloc(required_size + 1, sizeof(char));

    vsnprintf(buffer, required_size, format, args);

    esp_module_tx_put_line(buffer);

    free(buffer);
}

bool esp_module_rx_read()
{
    return (fifo_read(&esp_rx_fifo, &esp_rx_data, 1) == 1);
}

bool esp_module_rx_read_wait()
{
    return esp_module_rx_read_wait_timeout(500);
}

bool esp_module_rx_read_wait_timeout(const int32_t timeout_ms)
{
    int32_t read_start_ms = timeguard_get_time_ms();

    while (!esp_module_rx_char_ready())
    {
        if (timeguard_get_time_ms() - read_start_ms > timeout_ms)
        {
            console_put_formatted("ESP> Error: ESP module rx wait timed out after %i ms", (int)timeout_ms);
            return false;
        }
    }

    return esp_module_rx_read();
}

char *esp_module_rx_read_line(size_t max_length, int32_t timeout_ms)
{
    char *response = calloc(max_length + 1, sizeof(char));
    int response_index = 0;

    do
    {
        if (!esp_module_rx_read_wait_timeout(timeout_ms))
        {
            console_put_line("ESP> Error: Failed to read line. Did not receive a character in proper time interval.");

            free(response);
            return NULL;
        }

        if (esp_rx_data != '\n')
        {
            response[response_index++] = esp_rx_data;
        }
        else
        {
            return response;
        }

        if (response_index > max_length)
        {
            console_put_line("ESP> Error: Failed to read line. Response is too long.");

            free(response);
            return NULL;
        }
    } while (1);

    console_put_line("ESP> Error: Failed to read line. Unknown error.");

    free(response);
    return NULL;
}

bool esp_module_rx_char_ready()
{
    return fifo_has_next_item(&esp_rx_fifo);
}

void esp_module_rx_wait_until_char_ready()
{
    while (!fifo_has_next_item(&esp_rx_fifo))
    {
    }
}

void esp_module_clear_status()
{
    esp_module_rx_clear_queue();
    usart_reset_status(USART0);
}

void esp_module_rx_clear_queue()
{
    fifo_discard(&esp_rx_fifo);
}

uint8_t esp_module_rx_get_char()
{
    uint8_t buf;

    fifo_read(&esp_rx_fifo, &buf, 1);

    return buf;
}

uint8_t esp_module_rx_peek_char()
{
    uint8_t buf;

    fifo_peek(&esp_rx_fifo, &buf, 1);

    return buf;
}

void USART0_Handler()
{
    static char line_buff[255];
    static int line_idx = 0;

    // Handle RX situations by putting received bytes into fifo.
    if (USART0->US_IMR & US_IMR_RXRDY)
    {
        uint8_t recv = USART0->US_RHR;

        if (!audio_player_buffering) {
            fifo_write_single(&esp_rx_fifo, recv);

            if (recv != '\n') {
                *(line_buff + line_idx) = recv;
                line_idx++;
            }

            if (recv == '\n' || line_idx >= 250) {
                console_put_formatted("ESP(RX)> %s", line_buff);

                line_idx = 0;
                memset(line_buff, 0, sizeof(line_buff));
            }
        } else {
            fifo_write_single(audio_player_buffer, recv);
            audio_player_buffered_samples++;
            //audio_player_buffering_samples_left--;

            // if (audio_player_buffering_samples_left < 0) {
            //     audio_player_buffering = false;
            // }
        }
    }
}

__EXTERN_C_END
