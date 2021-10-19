#pragma once

#include "SerialPortController.h"

#include <string>
#include <mutex>
#include <memory>
#include <exception>

using Command = std::vector<std::string>;

class InvalidResponseException: public std::exception
{
public:
    InvalidResponseException(const std::string& what) :
            _what(what)
    {
    }
    InvalidResponseException() = default;
    const char* what()
    {
        return _what.c_str();
    }
private:
    std::string _what;
};

class SerialSendingException: public std::exception
{
public:
    SerialSendingException(const std::string& what) :
            _what(what)
    {
    }
    SerialSendingException() = default;
    const char* what()
    {
        return _what.c_str();
    }
private:
    std::string _what;
};

class SerialDeviceBase
{
public:
    SerialDeviceBase(const std::string& deviceId, bool isRf, std::shared_ptr<SerialPortController> serialPort);

    void responseHandler(const std::string& response);
    virtual std::string getId() const
    {
        return _deviceId;
    }
    virtual ~SerialDeviceBase() = default;

protected:
    void sendSimpleCommand(const Command& command);
    Command sendCommand(const Command& command);
    Command trySendCommand(const std::string& ravCommand);
    static Command parseResponse(const std::string& response);

    const std::string _deviceId;
    const bool _isRf;
    std::atomic<bool> _isResponseAvailable { false };
    Command _response;
    std::mutex _receivingMutex;
    std::shared_ptr<SerialPortController> _serialPort;

};
