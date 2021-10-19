#pragma once

#include "SerialPortController.h"
#include "SerialDeviceBase.h"

#include <string>
#include <unordered_map>
#include <memory>

class CannotGetDeviceIdException: public std::exception
{
public:
    CannotGetDeviceIdException() = default;
};

class SerialDevicesManager
{
public:
    void addDevice(std::shared_ptr<SerialDeviceBase> device)
    {
        _devices.emplace(device->getId(), device);
    }

    void responseHandler(const std::string& response, bool idRf);

private:
    static std::string getDeviceId(const std::string& ravResponse);
    std::unordered_map<std::string, std::shared_ptr<SerialDeviceBase>> _devices;
};
