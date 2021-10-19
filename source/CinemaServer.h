#pragma once

#include "SerialDevicesManager.h"
#include "HttpServer.h"

#include <cstdint>
#include <memory>

class SerialPortController;
class DeviceLedRGB;
class DeviceRF;
class DeviceScreen;

using CommandFunc = std::function<void()>;

class CinemaServer
{
public:
    CinemaServer(uint16_t httpPort, std::string httpFolderPath);
    void start();
private:
    void startShowing();
    void stopShowing();
    void volUp();
    void volDown();
    void pause();
    void play();
    void lightDown();
    void lightUp();
    void rollerFold();
    void rollerUnfold();

    void httpCommandHandler(const std::string& command);
    static void parseRequestJson(std::string& jsonString);

    HttpServer _httpServer;
    SerialDevicesManager _serialDevicesManager;
    std::shared_ptr<SerialPortController> _serialPortController;
    std::shared_ptr<DeviceLedRGB> _deviceLedRgb;
    std::shared_ptr<DeviceRF> _deviceRf;
    std::shared_ptr<DeviceScreen> _deviceScreen;
    std::unordered_map<std::string, CommandFunc> _commandsMap;

    bool _isPlaying { false };
};
