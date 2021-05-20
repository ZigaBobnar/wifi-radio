#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SD.h>
#include <ESP8266HTTPClient.h>

#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputSerialWAV.h"

void run_wait_for_init(void);
void run_wait_for_command(void);
void run_retrieve_command_data(void);
void run_stream(void);
void run_command_connect(String ssid, String password);
void run_command_start_stream(void);
void run_command_stop_stream(void);

//const char* g_stream_url = "http://live.radio1.si/Radio1";
const char* g_stream_url = "http://jazz.streamr.ru/jazz-64.mp3";

AudioGeneratorMP3 *g_mp3;
AudioFileSourceICYStream *g_file;
AudioFileSourceBuffer *g_buff;
AudioOutputSerialWAV *g_out;
bool g_is_radio_streaming = false;

enum class program_state {
  wait_for_command,
  retrieve_command_data,
};

enum {
  command_no_op = '0',
  command_status = 's',
  command_connect = 'c',
  command_get_connection_info = 'i',
  command_start_stream = 'S',
  command_stop_stream = 'E',
};

program_state g_state = program_state::wait_for_command;
uint8_t g_command = command_no_op;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string) {
    const char *ptr = reinterpret_cast<const char *>(cbData);
    (void) isUnicode; // Punt this ball for now
    // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
    char s1[32], s2[64];
    strncpy_P(s1, type, sizeof(s1));
    s1[sizeof(s1)-1]=0;
    strncpy_P(s2, string, sizeof(s2));
    s2[sizeof(s2)-1]=0;
    //Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
    //Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string) {
    const char *ptr = reinterpret_cast<const char *>(cbData);
    // Note that the string may be in PROGMEM, so copy it to RAM for printf
    char s1[64];
    strncpy_P(s1, string, sizeof(s1));
    s1[sizeof(s1)-1]=0;
    //Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
    //Serial.flush();
}



void setup() {
  Serial.begin(115200);
    delay(200);

  WiFi.disconnect();
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);

  while (!Serial) {}
}

void loop() {
  switch (g_state) {
    case program_state::wait_for_command:
      run_wait_for_command();
      break;
    case program_state::retrieve_command_data:
      run_retrieve_command_data();
      break;
  }

  if (g_is_radio_streaming) {
      run_stream();
  }
}

void run_wait_for_command() {
  int serial_available = Serial.available();

  if (serial_available > 0) {
    g_command = Serial.read();
    g_state = program_state::retrieve_command_data;
  }
}

void run_retrieve_command_data(void) {
  program_state next_state = program_state::wait_for_command;

  switch (g_command) {
    case command_no_op:
      break;
    case command_status: {
      Serial.write("OK");

      break;
    }
    case command_connect: {
      uint8_t ssid_length, password_length;
      
      while (!Serial.available()) {}
      ssid_length = Serial.read();
      while (!Serial.available()) {}
      password_length = Serial.read();
      
      String ssid = "", password = "";
      uint8_t ssid_read_index = 0, password_read_index = 0;

      do {
        while (!Serial.available()) {}
        ssid += Serial.read();
      } while (ssid_read_index <= ssid_length);

      do {
        while (!Serial.available()) {}
        password += Serial.read();
      } while (password_read_index <= password_length);

      run_command_connect(ssid, password);

      break;
    }
    case command_get_connection_info:
      // TODO

      break;
    case command_start_stream:
      // TODO: Stream args

      run_command_start_stream();

      break;
    case command_stop_stream:
      run_command_stop_stream();

      break;
  }

  g_state = next_state;
}

void run_stream(void) {
  if (g_mp3->isRunning()) {
    if (!g_mp3->loop()) g_mp3->stop();
  }
}

void run_command_connect(String ssid, String password) {
  WiFi.begin(ssid, password);

  do {
    wl_status_t status = WiFi.status();

    if (status == wl_status_t::WL_CONNECT_FAILED) {
      Serial.write(0x00);
      return;
    } else if (status == wl_status_t::WL_CONNECTED) {
      Serial.write(0x01);
      return;
    }

    delay(100);
  } while (true);
}

void run_command_start_stream() {
  audioLogger = &Serial;

  g_file = new AudioFileSourceICYStream(g_stream_url);
  g_file->RegisterMetadataCB(MDCallback, (void*)"ICY");
  g_buff = new AudioFileSourceBuffer(g_file, 8192);
  g_buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
  g_out = new AudioOutputSerialWAV();
  g_mp3 = new AudioGeneratorMP3();
  g_mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  g_mp3->begin(g_buff, g_out);

  
  g_is_radio_streaming = true;
}

void run_command_stop_stream(void) {
  g_mp3->stop();
  
  g_is_radio_streaming = false;
}
