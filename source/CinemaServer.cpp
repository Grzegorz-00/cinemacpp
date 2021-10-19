#include "CinemaServer.h"

#include "SerialPortController.h"
#include "DeviceLedRGB.h"
#include "DeviceRF.h"
#include "DeviceScreen.h"
#include "HttpServer.h"
#include "Logger.h"

#include <iostream>
#include <string_view>

namespace
{
constexpr uint8_t PAUSE_COLOR[3] = { 128, 255, 0 };
constexpr uint8_t PLAY_COLOR[3] = { 128, 255, 0 };

constexpr const char *LOG_SOURCE_ID = "CINEMA_SERVER";
}

CinemaServer::CinemaServer(uint16_t httpPort, std::string httpFolderPath) :
        _httpServer(httpPort, httpFolderPath, std::bind(&CinemaServer::httpCommandHandler, this, std::placeholders::_1))
{
    _serialPortController = std::make_shared<SerialPortController>(
            std::bind(&SerialDevicesManager::responseHandler, &_serialDevicesManager, std::placeholders::_1,
                    std::placeholders::_2));
    _deviceLedRgb = std::make_shared<DeviceLedRGB>(_serialPortController);
    _serialDevicesManager.addDevice(_deviceLedRgb);

    _deviceRf = std::make_shared<DeviceRF>(_serialPortController);
    _serialDevicesManager.addDevice(_deviceRf);

    _deviceScreen = std::make_shared<DeviceScreen>(_serialPortController);
    _serialDevicesManager.addDevice(_deviceScreen);

    _commandsMap.emplace("START_SHOWING", std::bind(&CinemaServer::startShowing, this));
    _commandsMap.emplace("STOP_SHOWING", std::bind(&CinemaServer::stopShowing, this));
    _commandsMap.emplace("VOL_UP", std::bind(&CinemaServer::volUp, this));
    _commandsMap.emplace("VOL_DOWN", std::bind(&CinemaServer::volDown, this));
    _commandsMap.emplace("PAUSE", std::bind(&CinemaServer::pause, this));
    _commandsMap.emplace("PLAY", std::bind(&CinemaServer::play, this));
    _commandsMap.emplace("LIGHT_DOWN", std::bind(&CinemaServer::lightDown, this));
    _commandsMap.emplace("LIGHT_UP", std::bind(&CinemaServer::lightUp, this));
    _commandsMap.emplace("ROLLER_UNFOLD", std::bind(&CinemaServer::rollerUnfold, this));
    _commandsMap.emplace("ROLLER_FOLD", std::bind(&CinemaServer::rollerFold, this));
}

void CinemaServer::httpCommandHandler(const std::string& command)
{
    if (_commandsMap.find(command) == _commandsMap.end())
    {
        Logger::getInstance().print(Logger::Level::WARNING, LOG_SOURCE_ID, "Unknown command: ", command);
    }

    _commandsMap[command]();
}

void CinemaServer::start()
{
    Logger::getInstance().print(Logger::Level::INFO, LOG_SOURCE_ID, "Starting cinemaServer");
    _httpServer.start();
}

void CinemaServer::startShowing()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call startShowing()");
    _deviceRf->setBrightnessSlowly(2, 0, 4);
    _deviceScreen->unfoldScreen();
    _deviceScreen->waitForUnfolded(15000);
    _deviceScreen->turnOnProjector();
    uint8_t color[3] = { 64, 128, 0 };
    _deviceLedRgb->setRGBSlowly(0, color);
    _deviceLedRgb->setRGBSlowly(1, color);

    _isPlaying = false;
}

void CinemaServer::stopShowing()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call stopShowing()");
    _deviceScreen->turnOffProjector();
    _deviceScreen->foldScreen();
    uint8_t color[3] = { 0, 0, 0 };
    _deviceLedRgb->setRGBSlowly(0, color);
    _deviceLedRgb->setRGBSlowly(1, color);
    _deviceRf->setBrightnessSlowly(2, 255, 1);
}

void CinemaServer::volUp()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call volUp()");
}

void CinemaServer::volDown()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call volDown()");
}

void CinemaServer::pause()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call pause()");
    _isPlaying = false;
    _deviceLedRgb->setRGBSlowly(0, PAUSE_COLOR);
    _deviceLedRgb->setRGBSlowly(1, PAUSE_COLOR);
}

void CinemaServer::play()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call play()");
    _isPlaying = true;
    _deviceLedRgb->setRGBSlowly(0, PLAY_COLOR);
    _deviceLedRgb->setRGBSlowly(1, PLAY_COLOR);
}

void CinemaServer::lightDown()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call lightDown()");
    _deviceRf->lightDown(40);
}

void CinemaServer::lightUp()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call lightUp()");
    _deviceRf->lightUp(40);
}

void CinemaServer::rollerFold()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call rollerFold()");
    _deviceRf->windowsRollerFold();
}

void CinemaServer::rollerUnfold()
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call rollerUnfold()");
    _deviceRf->windowsRollerUnfold();
}

