#pragma once

#include <AudioOutput.h>

class SerialWavOutput : public AudioOutput
{
  public:
    SerialWavOutput() {};
    ~SerialWavOutput() {};
    virtual bool begin() override;
    virtual bool ConsumeSample(int16_t sample[2]) override;
    virtual bool stop() override;
  private:
    int count;
};
