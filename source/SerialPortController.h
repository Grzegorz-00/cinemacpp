#pragma once

#include <string>
#include <optional>
#include <functional>
#include <thread>
#include <atomic>

#include "serial/serial.h"

using CommandHandlerFunction = std::function<void(const std::string&, bool)>;

class SerialPortController
{
public:
    SerialPortController(CommandHandlerFunction fun);
    ~SerialPortController();

    SerialPortController(const SerialPortController&) = delete;
    SerialPortController& operator=(const SerialPortController&) = delete;

    void sendRavMessage(std::string, bool isRf);

private:
    void checkForCommandTask();
    void checkForCommandFromDevice(serial::Serial& device, std::string& buffer);
    void extractCommands(std::string& buffer);
    void discoverDevices();
    void checkDevices(const std::string& port);
    bool checkSingleDevice(const std::string& port, const std::string& request, const std::string& response);
    serial::Serial _radioSerialDevice;
    serial::Serial _cableSerialDevice;
    std::string _radioBuffer;
    std::string _cableBuffer;
    CommandHandlerFunction _recFun;
    std::atomic<bool> _isThreadRunning { false };
    std::thread _waitingForCommandThread;

};
