#include "DeviceRF.h"

DeviceRF::DeviceRF(std::shared_ptr<SerialPortController> serialPort) :
        SerialDeviceBase("RF_DRIVER", true, serialPort)
{
}

void DeviceRF::updateBrightness(uint8_t channel, uint8_t brightness)
{
    if (channel > 1)
        _lampBrightness[0] = _lampBrightness[1] = brightness;
    else
        _lampBrightness[channel] = brightness;
}

void DeviceRF::setBrightness(uint8_t channel, uint8_t brightness)
{
    updateBrightness(channel, brightness);
    Command command;
    command.emplace_back("SET_BRIGHTNESS");
    command.emplace_back(std::to_string(channel));
    command.emplace_back(std::to_string(brightness));
    sendSimpleCommand(command);
}

void DeviceRF::setBrightnessSlowly(uint8_t channel, uint8_t brightness, uint8_t speed)
{
    updateBrightness(channel, brightness);
    Command command;
    command.emplace_back("LD-SET_BRIGHTNESS_SLOWLY");
    command.emplace_back(std::to_string(channel));
    command.emplace_back(std::to_string(brightness));
    command.emplace_back(std::to_string(speed));
    sendSimpleCommand(command);
}

void DeviceRF::lightDown(uint8_t step)
{
    for (uint8_t i = 0; i < 2; i++)
    {
        int16_t tempBr = _lampBrightness[i] - step;
        if (tempBr < 0)
            _lampBrightness[i] = 0;
        else
            _lampBrightness[i] = tempBr;

        setBrightnessSlowly(i, _lampBrightness[i], 4);
    }
}

void DeviceRF::lightUp(uint8_t step)
{
    for (uint8_t i = 0; i < 2; i++)
    {
        int16_t tempBr = _lampBrightness[i] + step;
        if (tempBr > 255)
            _lampBrightness[i] = 255;
        else
            _lampBrightness[i] = tempBr;

        setBrightnessSlowly(i, _lampBrightness[i], 4);
    }
}

void DeviceRF::windowsRollerUnfold()
{
    Command command;
    command.emplace_back("UNFOLD");
    command.emplace_back(std::to_string(0));
    sendSimpleCommand(command);
}

void DeviceRF::windowsRollerFold()
{
    Command command;
    command.emplace_back("FOLD");
    command.emplace_back(std::to_string(0));
    sendSimpleCommand(command);
}

