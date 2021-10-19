#pragma once

#include "SerialDeviceBase.h"

class DeviceRF: public SerialDeviceBase
{
public:
    DeviceRF(std::shared_ptr<SerialPortController> serialPort);
    void setBrightness(uint8_t channel, uint8_t brightness);
    void setBrightnessSlowly(uint8_t channel, uint8_t brightness, uint8_t speed);
    void lightDown(uint8_t step);
    void lightUp(uint8_t step);

    void windowsRollerUnfold();
    void windowsRollerFold();
private:
    void updateBrightness(uint8_t channel, uint8_t brightness);
    uint8_t _lampBrightness[2] { 255, 255 };
};
