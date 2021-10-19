#pragma once

#include "SerialDeviceBase.h"

class DeviceLedRGB: public SerialDeviceBase
{
public:
    DeviceLedRGB(std::shared_ptr<SerialPortController>& serialPort);
    void setRGB(const uint8_t channel, const uint8_t color[3]);
    void setRGBSlowly(const uint8_t channel, const uint8_t color[3]);

};
