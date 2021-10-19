#include "DeviceLedRGB.h"

DeviceLedRGB::DeviceLedRGB(std::shared_ptr<SerialPortController>& serialPort) :
        SerialDeviceBase("LED_DRIVER_1", false, serialPort)
{

}

void DeviceLedRGB::setRGB(const uint8_t channel, const uint8_t color[3])
{
    Command command;
    command.emplace_back("SET_RGB");
    command.emplace_back(std::to_string(channel));
    command.emplace_back(std::to_string(color[0]));
    command.emplace_back(std::to_string(color[1]));
    command.emplace_back(std::to_string(color[2]));
    sendSimpleCommand(command);
}

void DeviceLedRGB::setRGBSlowly(const uint8_t channel, const uint8_t color[3])
{
    Command command;
    command.emplace_back("SET_RGB_SLOWLY");
    command.emplace_back(std::to_string(channel));
    command.emplace_back(std::to_string(color[0]));
    command.emplace_back(std::to_string(color[1]));
    command.emplace_back(std::to_string(color[2]));
    sendSimpleCommand(command);
}
