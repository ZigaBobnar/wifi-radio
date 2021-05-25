#pragma once

#include <Arduino.h>
#include <ESP8266HTTPClient.h>

class NetworkWavClient {
  public:
    NetworkWavClient();
    NetworkWavClient(const char* url);
    virtual ~NetworkWavClient();

    bool connect(const char* url);

    uint32_t read(void* data, uint32_t length);

    void disconnect();

  private:
    WiFiClient _client;
    HTTPClient _http;
    int _position;
    int _size;
    char _stream_url[128];
};
