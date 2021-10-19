#include "SerialDevicesManager.h"

#include "Logger.h"

#include <iostream>

namespace
{
constexpr const char *LOG_SOURCE_ID = "SER_DEV_MAN";
}

void SerialDevicesManager::responseHandler(const std::string& response, bool /*idRf*/)
{
    std::string deviceId = getDeviceId(response);
    if (_devices.find(deviceId) == _devices.end())
    {

        Logger::getInstance().print(Logger::Level::WARNING, LOG_SOURCE_ID, "Unknown device: ", deviceId);
        return;
    }

    _devices[deviceId]->responseHandler(response);
}

std::string SerialDevicesManager::getDeviceId(const std::string& ravResponse)
{
    size_t idx = ravResponse.find(':');
    if (idx == std::string::npos)
    {
        throw CannotGetDeviceIdException();
    }

    return ravResponse.substr(0, idx);
}
