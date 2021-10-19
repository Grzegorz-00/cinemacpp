#include "SerialDeviceBase.h"

#include "Logger.h"

#include <sstream>
#include <iostream>
#include <cstdint>
#include <exception>

namespace
{
constexpr uint8_t SENDING_RETRIES = 5;
constexpr uint16_t SINGLE_SEND_TIMEOUT_MS = 100;
constexpr uint16_t RESPONSE_CHECKING_INTERVAL_MS = 10;
constexpr const char *LOG_SOURCE_ID = "SER_BASE_DEV";
}

SerialDeviceBase::SerialDeviceBase(const std::string& deviceId, bool isRf,
        std::shared_ptr<SerialPortController> serialPort) :
        _deviceId(deviceId), _isRf(isRf), _serialPort(serialPort)
{

}

void SerialDeviceBase::responseHandler(const std::string& response)
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call responseHandler() response: [", response,
            ']');
    if (!_isResponseAvailable.load())
    {
        std::scoped_lock<std::mutex> lock(_receivingMutex);
        _response = parseResponse(response);
        _isResponseAvailable.store(true);
    }
}

Command SerialDeviceBase::parseResponse(const std::string& response)
{
    Command command;

    std::istringstream iStream(response.substr(1, response.size() - 1));
    std::string temp;
    while (std::getline(iStream, temp, ':'))
    {
        command.push_back(temp);
    }

    return command;
}

void SerialDeviceBase::sendSimpleCommand(const Command& command)
{
    Command response = sendCommand(command);
    if (response.size() < 2)
    {
        Logger::getInstance().print(Logger::Level::ERROR, LOG_SOURCE_ID, "Invalid response");
        //throw SerialSendingException("Invalid response");
    }
    else
    {
        if (response[1].compare("OK") != 0)
        {
            Logger::getInstance().print(Logger::Level::ERROR, LOG_SOURCE_ID, "No ok response: [", response[1], ']');
            //throw SerialSendingException("No OK response");
        }
    }
}

Command SerialDeviceBase::sendCommand(const Command& command)
{
    std::ostringstream strStream;
    strStream << _deviceId << ':';
    for (const auto parameter : command)
    {
        strStream << parameter << ':';
    }
    for (uint8_t retNum = 0; retNum < SENDING_RETRIES; retNum++)
    {
        try
        {
            if (retNum > 1)
            {
                Logger::getInstance().print(Logger::Level::WARNING, LOG_SOURCE_ID, "Trying send command. Attempt no: ",
                        (int) retNum);
            }
            return trySendCommand(strStream.str());
        }
        catch (std::exception &ex)
        {
        }
    }
    Logger::getInstance().print(Logger::Level::ERROR, LOG_SOURCE_ID, "Cannot send command");
    //throw SerialSendingException("Cannot send command");
    Command com;
    return com;
}

Command SerialDeviceBase::trySendCommand(const std::string& ravCommand)
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, " -> ", ravCommand);

    _isResponseAvailable.store(false);
    _serialPort->sendRavMessage(ravCommand, _isRf);

    for (uint16_t waitingTimeMs = 0; waitingTimeMs < SINGLE_SEND_TIMEOUT_MS; waitingTimeMs +=
            RESPONSE_CHECKING_INTERVAL_MS)
    {
        if (_isResponseAvailable.load())
        {
            _isResponseAvailable.store(false);
            std::scoped_lock<std::mutex> lock(_receivingMutex);
            return std::move(_response);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(RESPONSE_CHECKING_INTERVAL_MS));
    }
    throw SerialSendingException("Cannot send command");
}
