#include "network_wav_client.h"

NetworkWavClient::NetworkWavClient() {
    _stream_url[0] = 0;
}

NetworkWavClient::NetworkWavClient(const char* url) {
    _stream_url[0] = 0;

    connect(url);
}

NetworkWavClient::~NetworkWavClient() {
    _http.end();
}

bool NetworkWavClient::connect(const char* url) {
    _http.begin(_client, url);
    _http.setReuse(true);

    int code = _http.GET();
    if (code != HTTP_CODE_OK) {
        _http.end();

        // HTTP request failed

        return false;
    }

    _size = _http.getSize();
    strncpy(_stream_url, url, sizeof(_stream_url));
    _stream_url[sizeof(_stream_url) - 1] = 0;

    return true;
}

uint32_t NetworkWavClient::read(void* data, uint32_t length) {
    if (data == nullptr) {
        return 0;
    }

    if (!_http.connected()) {
        _http.end();
        // TODO: Try to reconnect
        return 0;
    }

    if ((_size > 0) && (_position >= _size)) return 0;

    WiFiClient *stream = _http.getStreamPtr();

    if ((_size > 0) && (length > (uint32_t)(_position - _size)))
        length = _position - _size;

    size_t stream_available = stream->available();
    if (stream_available == 0)
        return 0;

    if (stream_available < length)
        length = stream_available;

    int read = stream->read(reinterpret_cast<uint8_t*>(data), length);

    _position += read;

    return read;
}

void NetworkWavClient::disconnect() {
    _http.end();
}
