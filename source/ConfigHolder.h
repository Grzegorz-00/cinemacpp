#pragma once

#include <cstdint>
#include <string>

class ConfigHolder
{
public:
    static ConfigHolder& getInstance()
    {
        static ConfigHolder configHolder;
        return configHolder;
    }

    bool init(int argc, char** argv)
    {
        if (_isInited)
            return true;

        if (parseArgs(argc, argv))
            _isInited = true;
        else
            return false;

        return true;
    }

    const uint16_t getHttpPort() const
    {
        return _httpPort;
    }
    const std::string getHttpFolderPath() const
    {
        return _httpFolderPath;
    }
    const bool getIsDebugLogs() const
    {
        return _isDebugLogs;
    }
private:
    ConfigHolder() = default;
    bool parseArgs(int argc, char** argv);

    bool _isInited { false };

    uint16_t _httpPort { 0 };
    std::string _httpFolderPath {""};
    bool _isDebugLogs { false };
};
