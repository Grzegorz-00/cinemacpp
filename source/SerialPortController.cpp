#include "SerialPortController.h"

#include "Logger.h"

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>

namespace
{
constexpr int BAUDRATE = 115200;
constexpr const char *DEVICE_PATTERN = "ttyUSB";

constexpr const char *RF_DEVICE_REQUEST = "RF_DRIVER:RB-GET_STATUS:\r\n";
constexpr const char *RF_DEVICE_RESPONSE = "RF_DRIVER:OK\r\n";
constexpr const char *CABLE_DEVICE_REQUEST = "SCREEN_DRIVER_1:GET_STATUS:\r\n";
constexpr const char *CABLE_DEVICE_RESPONSE = "SCREEN_DRIVER_1:OK\r\n";

constexpr int DISCOVER_RESPONSE_TIMEOUT_MS = 2000;
constexpr int READ_BUFFER_SIZE = 1024;
constexpr int THREAD_SLEEP_MS = 10;
constexpr char MESSAGE_NEWLINE = '\n';

constexpr const char *LOG_SOURCE_ID = "SER_POR_CONTRL";
}

SerialPortController::SerialPortController(CommandHandlerFunction fun) :
        _recFun(fun)
{
    discoverDevices();
    _isThreadRunning.store(true);
    _waitingForCommandThread = std::thread(&SerialPortController::checkForCommandTask, this);
}

SerialPortController::~SerialPortController()
{
    _isThreadRunning.store(false);
    _waitingForCommandThread.join();
}

void SerialPortController::checkForCommandTask()
{
    while (_isThreadRunning.load())
    {
        checkForCommandFromDevice(_radioSerialDevice, _radioBuffer);
        checkForCommandFromDevice(_cableSerialDevice, _cableBuffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_MS));
    }
}

void SerialPortController::checkForCommandFromDevice(serial::Serial& device, std::string& buffer)
{
    if (!device.isOpen())
        return;

    const size_t currentBufferSize = buffer.size();
    const size_t deviceDataSize = device.available();

    if (deviceDataSize == 0)
        return;

    if (currentBufferSize + deviceDataSize > READ_BUFFER_SIZE)
    {
        if (deviceDataSize < READ_BUFFER_SIZE)
        {

            buffer.erase(0, READ_BUFFER_SIZE - deviceDataSize);
            std::string newData = device.read(deviceDataSize);
            buffer += newData;
        }
        else
        {
            buffer = device.read(READ_BUFFER_SIZE);
        }
    }
    else
    {
        std::string newData = device.read(deviceDataSize);
        buffer += newData;
    }

    extractCommands(buffer);
}

void SerialPortController::extractCommands(std::string& buffer)
{
    while (true)
    {
        const size_t pos = buffer.find(MESSAGE_NEWLINE);
        if (pos == std::string::npos)
            return;

        if (pos > 0)
        {
            Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, " <- [", buffer.substr(0, pos - 1), ']');
            _recFun(buffer.substr(0, pos - 1), false);
        }

        buffer.erase(0, pos + 1);
    }
}

void SerialPortController::discoverDevices()
{
    std::vector<serial::PortInfo> devicesFound = serial::list_ports();

    for (const auto device : devicesFound)
    {
        if (device.port.find(DEVICE_PATTERN) != std::string::npos)
        {
            checkDevices(device.port);
        }

        if (_radioSerialDevice.isOpen() && _cableSerialDevice.isOpen())
            break;
    }

    if (_radioSerialDevice.isOpen())
        Logger::getInstance().print(Logger::Level::INFO, LOG_SOURCE_ID, "RF device found at: ",
                _radioSerialDevice.getPort());
    else
        Logger::getInstance().print(Logger::Level::WARNING, LOG_SOURCE_ID, "RF device not found");

    if (_cableSerialDevice.isOpen())
        Logger::getInstance().print(Logger::Level::INFO, LOG_SOURCE_ID, "Cable device found at: ",
                _cableSerialDevice.getPort());
    else
        Logger::getInstance().print(Logger::Level::WARNING, LOG_SOURCE_ID, "Cable device not found");
}

void SerialPortController::checkDevices(const std::string& port)
{
    if (!_radioSerialDevice.isOpen())
    {
        if (checkSingleDevice(port, RF_DEVICE_REQUEST, RF_DEVICE_RESPONSE))
        {
            _radioSerialDevice.setPort(port);
            _radioSerialDevice.setBaudrate(BAUDRATE);
            _radioSerialDevice.open();
            return;
        }
    }

    if (!_cableSerialDevice.isOpen())
    {
        if (checkSingleDevice(port, CABLE_DEVICE_REQUEST, CABLE_DEVICE_RESPONSE))
        {
            _cableSerialDevice.setPort(port);
            _cableSerialDevice.setBaudrate(BAUDRATE);
            _cableSerialDevice.open();
            return;
        }
    }
}

bool SerialPortController::checkSingleDevice(const std::string& port, const std::string& request,
        const std::string& response)
{
    try
    {
        Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "Checking port: ", port);
        serial::Serial device = serial::Serial(port, BAUDRATE);
        device.write(request);

        for (std::size_t time = 0; time < DISCOVER_RESPONSE_TIMEOUT_MS; time += 200)
        {
            if (device.available() == response.size())
            {
                std::string buffer;
                device.read(buffer, device.available());
                device.close();
                if (buffer.compare(response) == 0)
                {
                    return true;
                }
                Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "Response mismatch: ", buffer, " != ",
                        response);
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

    }
    catch (const std::exception &ex)
    {
    }
    return false;
}

void SerialPortController::sendRavMessage(std::string data, bool isRf)
{
    if (isRf)
    {
        if (_radioSerialDevice.isOpen())
        {
            _radioSerialDevice.write(data);
            _radioSerialDevice.write("\r\n");
        }
    }
    else
    {
        if (_cableSerialDevice.isOpen())
        {
            _cableSerialDevice.write(data);
            _cableSerialDevice.write("\r\n");
        }
    }
}
