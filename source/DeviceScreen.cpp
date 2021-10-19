#include "DeviceScreen.h"

#include "Logger.h"

namespace
{
constexpr uint16_t CHECKING_STATE_INTERVAL_MS = 500;
constexpr const char *LOG_SOURCE_ID = "DEV_SCREEN";
}

DeviceScreen::DeviceScreen(std::shared_ptr<SerialPortController> serialPort) :
        SerialDeviceBase("SCREEN_DRIVER_1", false, serialPort)
{

}

DeviceScreen::State DeviceScreen::stringToState(const std::string& stringState)
{
    if (stringState.compare("STATE=FOLDING") == 0)
        return State::FOLDING;
    else if (stringState.compare("STATE=FOLDED") == 0)
        return State::FOLDED;
    else if (stringState.compare("STATE=UNFOLDING") == 0)
        return State::UNFOLDING;
    else if (stringState.compare("STATE=UNFOLDED") == 0)
        return State::UNFOLDED;
    else if (stringState.compare("STATE=ERROR") == 0)
        return State::ERROR;

    return State::INVALID;
}

std::string DeviceScreen::stateToString(State state)
{
    switch (state)
    {
    case State::FOLDING:
        return "FOLDING";
    case State::FOLDED:
        return "FOLDED";
    case State::UNFOLDING:
        return "UNFOLDING";
    case State::UNFOLDED:
        return "UNFOLDED";
    case State::ERROR:
        return "ERROR";
    case State::INVALID:
        return "INVALID";
    }
    return "INVALID";
}

DeviceScreen::State DeviceScreen::getScreenState()
{
    Command command;
    command.emplace_back("GET_STATE");
    Command response = sendCommand(command);
    if (response.size() < 2)
        throw InvalidResponseException("Invalid getScreenState() response");

    State state = stringToState(response[1]);
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "Screen state: ", stateToString(state));
    return state;
}

void DeviceScreen::foldScreen()
{
    Command command;
    command.emplace_back("FOLD_SCREEN");
    sendSimpleCommand(command);
}

void DeviceScreen::unfoldScreen()
{
    Command command;
    command.emplace_back("UNFOLD_SCREEN");
    sendSimpleCommand(command);
}

void DeviceScreen::waitForUnfolded(uint16_t timeoutMs)
{
    for (uint32_t time = 0; time < timeoutMs; time += CHECKING_STATE_INTERVAL_MS)
    {
        if (getScreenState() == State::UNFOLDED)
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(CHECKING_STATE_INTERVAL_MS));
    }
}

void DeviceScreen::toggleProjector()
{
    Command command;
    command.emplace_back("TOGGLE_PROJECTOR");
    sendSimpleCommand(command);
}

void DeviceScreen::turnOnProjector()
{
    Command command;
    command.emplace_back("TURN_ON_PROJECTOR");
    sendSimpleCommand(command);
}

void DeviceScreen::turnOffProjector()
{
    Command command;
    command.emplace_back("TURN_OFF_PROJECTOR");
    sendSimpleCommand(command);
}
