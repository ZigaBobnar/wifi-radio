#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SD.h>
#include <ESP8266HTTPClient.h>

//#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceHTTPStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputSerialWAV.h"

#include "serial_wav_output.h"

void run_wait_for_init(void);
void run_wait_for_command(void);
void run_command(String command);
void run_stream(void);
void run_command_connect(String ssid, String password);
void run_command_start_stream(void);
void run_command_stop_stream(void);

//const char* g_stream_url = "http://live.radio1.si/Radio1";
//const char* g_stream_url = "http://jazz.streamr.ru/jazz-64.mp3";
const char* g_stream_url = "http://192.168.0.2:8000/radio";

AudioGeneratorMP3 *g_mp3;
//AudioFileSourceICYStream *g_file;
AudioFileSourceHTTPStream *g_file;
AudioFileSourceBuffer *g_buff;
SerialWavOutput *g_out;
bool g_is_radio_streaming = false;

String command_line;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string) {
    //const char *ptr = reinterpret_cast<const char *>(cbData);
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
    //const char *ptr = reinterpret_cast<const char *>(cbData);
    // Note that the string may be in PROGMEM, so copy it to RAM for printf
    char s1[64];
    strncpy_P(s1, string, sizeof(s1));
    s1[sizeof(s1)-1]=0;
    //Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
    //Serial.flush();
}



void setup() {
  WiFi.disconnect();
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);

  Serial.begin(/*74880*/115200);
  delay(200);

  command_line = "";

  //while (!Serial) {}
}

void loop() {
  run_wait_for_command();

  if (g_is_radio_streaming) {
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
  if (g_mp3->isRunning()) {
    if (!g_mp3->loop()) {
      g_mp3->stop();
      Serial.print("Stopped\n");
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
  //audioLogger = &Serial;
  g_file = new AudioFileSourceHTTPStream(g_stream_url);
  //g_file = new AudioFileSourceICYStream(g_stream_url);
  g_file->RegisterMetadataCB(MDCallback, (void*)"ICY");
  g_buff = new AudioFileSourceBuffer(g_file, /*32784*//*2048*/8192);
  g_buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
  g_out = new SerialWavOutput();
  //g_out->SetChannels(1);
  //g_out->SetRate(500);
  //g_out->SetBitsPerSample(8);
  g_mp3 = new AudioGeneratorMP3();
  g_mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  g_mp3->begin(g_buff, g_out);

  g_is_radio_streaming = true;
}

void run_command_stop_stream(void) {
  g_mp3->stop();
  
  delete g_mp3;
  delete g_out;
  delete g_buff;
  delete g_file;

  g_is_radio_streaming = false;
}
