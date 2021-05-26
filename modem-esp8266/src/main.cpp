#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SD.h>
#include <ESP8266HTTPClient.h>

#include "network_wav_client.h"

void run_wait_for_init(void);
void run_wait_for_command(void);
void run_command(String command);
void run_command_connect(String ssid, String password);
void run_command_play_next();
void run_command_play_previous();
void run_command_get_currently_playing();
void run_command_get_track_info(int track_id);
void run_command_get_chunk(int track_id, int chunk_index);
void run_command_get_current_time();
// void run_stream(void);
// void run_command_start_stream(void);
// void run_command_stop_stream(void);

const String api_url = "http://192.168.0.2:20343";

// bool is_radio_streaming = false;

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

  /*if (is_radio_streaming) {
      run_stream();
  }*/
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
  } else if (command == "play_next") {
    run_command_play_next();
  } else if (command == "play_previous") {
    run_command_play_previous();
  } else if (command == "get_currently_playing") {
    run_command_get_currently_playing();
  } else if (command.startsWith("get_track_info ")) {
    String track_id;
    for (uint i = 15; i < command.length(); i++) {
      track_id += command[i];
    }

    if (track_id.isEmpty()) {
      Serial.print("FAIL\n");
      return;
    }

    int track_id_int = atoi(track_id.c_str());

    run_command_get_track_info(track_id_int);
  } else if (command.startsWith("get_chunk ")) {
    
    String track_id, chunk_index;
    bool reading_id = true;
    
    for (uint i = 8; i < command.length(); i++) {
      if (command[i] != ' ') {
        if (reading_id) {
          track_id += command[i];
        } else {
          chunk_index += command[i];
        }
      } else {
        if (reading_id) {
          reading_id = false;
        } else {
          break;
          // TODO: Too many arguments
        }
      }
    }

    if (track_id.isEmpty() || chunk_index.isEmpty()) {
      Serial.print("FAIL\n");
      return;
    }

    int track_id_int = atoi(track_id.c_str());
    int chunk_index_int = atoi(chunk_index.c_str());

    run_command_get_chunk(track_id_int, chunk_index_int);
  } else if (command == "get_current_time") {
    run_command_get_current_time();
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

void run_command_play_next() {
  HTTPClient http;
  http.begin(api_url + "/next");
  http.GET();
  http.end();
}

void run_command_play_previous() {
  HTTPClient http;
  http.begin(api_url + "/previous");
  http.GET();
  http.end();
}

void run_command_get_currently_playing() {
  HTTPClient http;
  http.begin(api_url + "/track/current");
  int statusCode = http.GET();
  if (statusCode == HTTP_CODE_OK) {
    String body = http.getString();

    Serial.printf("OK %s\n", body.c_str());
  } else {
    Serial.printf("FAIL\n");
  }

  http.end();
}

void run_command_get_track_info(int track_id) {
  HTTPClient http;
  String endpoint = String("/track/%id");
  endpoint.replace("%id", String(track_id));
  http.begin(api_url + endpoint);
  int statusCode = http.GET();
  if (statusCode == HTTP_CODE_OK) {
    String body = http.getString();
    
    Serial.printf("OK %s\n", body.c_str());
  } else {
    Serial.printf("FAIL\n");
  }

  http.end();
}

void run_command_get_chunk(int track_id, int chunk_index) {
  HTTPClient http;
  String endpoint = String("/track/%id/chunk/%chunkIndex");
  endpoint.replace("%id", String(track_id));
  endpoint.replace("%chunkIndex", String(chunk_index));
  http.begin(api_url + endpoint);
  int statusCode = http.GET();
  if (statusCode == HTTP_CODE_OK) {
    String body = http.getString();
    
    Serial.printf("OK %i\n", body.length());
    Serial.print(body);
  } else {
    Serial.printf("FAIL\n");
  }

  http.end();
}

void run_command_get_current_time() {
  HTTPClient http;
  http.begin(api_url + "/time/now");
  int statusCode = http.GET();
  if (statusCode == HTTP_CODE_OK) {
    String body = http.getString();

    Serial.printf("OK %s\n", body.c_str());
  } else {
    Serial.printf("FAIL\n");
  }

  http.end();
}

/*void run_command_start_stream() {
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
}*/
