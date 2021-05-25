#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SD.h>
#include <ESP8266HTTPClient.h>

//#include "AudioFileSourceICYStream.h"
//#include "AudioFileSourceHTTPStream.h"
//#include "AudioFileSourceBuffer.h"
//#include "AudioGeneratorMP3.h"
//#include "AudioOutputSerialWAV.h"

#include "network_wav_client.h"

void run_wait_for_init(void);
void run_wait_for_command(void);
void run_command(String command);
void run_stream(void);
void run_command_connect(String ssid, String password);
void run_command_start_stream(void);
void run_command_stop_stream(void);

//const char* g_stream_url = "http://live.radio1.si/Radio1";
//const char* g_stream_url = "http://jazz.streamr.ru/jazz-64.mp3";
const char* stream_url = "http://192.168.0.2:20343/stream";

bool is_radio_streaming = false;

String command_line;
NetworkWavClient* wav_client;

void setup() {
  WiFi.disconnect();
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);

  Serial.begin(/*74880*//*115200*/500000);
  delay(200);

  command_line = "";

  //while (!Serial) {}
}

void loop() {
  run_wait_for_command();

  if (is_radio_streaming) {
      run_stream();
  }
}

void run_wait_for_command() {
  while (Serial.available()) {
    char value = Serial.read();

    if (value == '\n') {
      run_command(command_line);

      command_line = "";
    } else {
      command_line += value;
    }
  }
}

void run_command(String command) {
  if (command == "status") {
    Serial.print("OK\n");

  } else if (command.startsWith("connect ")) {
    String ssid, password;
    bool reading_ssid = true;
    
    for (uint i = 8; i < command.length(); i++) {
      if (command[i] != ' ') {
        if (reading_ssid) {
          ssid += command[i];
        } else {
          password += command[i];
        }
      } else {
        if (reading_ssid) {
          reading_ssid = false;
        } else {
          break;
          // TODO: Too many arguments
        }
      }
    }

    if (ssid.isEmpty()) {
      Serial.print("Connection failed. SSID is empty\n");
      return;
    }

    if (password.isEmpty()) {
      Serial.print("Connection failed. Password is empty\n");
      return;
    }

    run_command_connect(ssid, password);
  } else if (command == "start_stream") {
    run_command_start_stream();
  } else if (command == "stop_stream") {
    run_command_stop_stream();
  } else {
    // Unknown command
  }
}

void run_stream(void) {
  if (wav_client != nullptr) {
    uint8_t data[2048];
    uint32_t data_read = wav_client->read(&data, sizeof(data));

    for (uint32_t i = 0; i < data_read; ++i) {
      Serial.write(data[i]);
    }
  }
}

void run_command_connect(String ssid, String password) {
  WiFi.begin(ssid, password);

  int i = 0;
  do {
    delay(1000);

    wl_status_t status = WiFi.status();
    
    if (status == wl_status_t::WL_CONNECTED) {
      Serial.print("OK\n");
      return;
    } else if (status == wl_status_t::WL_CONNECT_FAILED) {
      Serial.print("Connection failed\n");
      return;
    }

    i++;
  } while (i < 10);

  Serial.print("Connection timed out\n");
}

void run_command_start_stream() {
  run_command_stop_stream();
  
  wav_client = new NetworkWavClient(stream_url);

  is_radio_streaming = true;
}

void run_command_stop_stream(void) {
  if (wav_client != nullptr) {
    wav_client->disconnect();
    delete wav_client;
  }

  is_radio_streaming = false;
}
