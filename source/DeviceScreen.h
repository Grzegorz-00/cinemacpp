#pragma once

#include "SerialDeviceBase.h"

#include <string>
#include <cstdint>

class DeviceScreen: public SerialDeviceBase
{
public:
    enum class State
    {
        FOLDING, FOLDED, UNFOLDING, UNFOLDED, ERROR, INVALID
    };

    DeviceScreen(std::shared_ptr<SerialPortController> serialPort);

    static State stringToState(const std::string& state);
    static std::string stateToString(State state);
    State getScreenState();
    void foldScreen();
    void unfoldScreen();
    void waitForUnfolded(uint16_t timeoutMs);

    void toggleProjector();
    void turnOnProjector();
    void turnOffProjector();
};
