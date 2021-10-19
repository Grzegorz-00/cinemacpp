#pragma once

#include <mutex>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace
{
constexpr uint8_t SOURCE_ID_CHAR_NUM = 15;
}

class Logger
{
public:
    enum class Level
    {
        DEBUG, INFO, WARNING, ERROR
    };

    static std::string levelToString(Level level)
    {
        switch (level)
        {
        case Level::DEBUG:
            return "DEBUG";
        case Level::INFO:
            return "INFO";
        case Level::WARNING:
            return "WARNING";
        case Level::ERROR:
            return "ERROR";
        }
        return "UNKNOWN";
    }

    static Logger& getInstance()
    {
        static Logger logger;
        return logger;
    }

    void setIsDebug(bool isDebug)
    {
        _isDebug = isDebug;
    }

    template<typename T, typename ... Targs>
    void print(Level level, const std::string& sourceId, const T& value, const Targs& ... args)
    {
        if (level == Level::DEBUG && !_isDebug)
            return;
        std::scoped_lock<std::mutex> lock(_iostreamMutex);
        printPrefix(level, sourceId);
        printImpl(value, args...);
        std::cout << std::endl;
    }

private:
    Logger() = default;
    std::string getTimeStamp()
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream sstream;
        sstream << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
        return sstream.str();
    }

    void printPrefix(Level level, const std::string& sourceId)
    {
        if (level == Level::DEBUG && !_isDebug)
            return;
        std::cout << getTimeStamp() << "[" << std::setw(7) << Logger::levelToString(level) << "]["
                << std::setw(SOURCE_ID_CHAR_NUM) << sourceId << ']';
    }

    void printImpl()
    {
    }

    template<typename T, typename ... Targs>
    void printImpl(const T& value, const Targs& ... args)
    {
        std::cout << value;
        printImpl(args...);
    }

    std::mutex _iostreamMutex;
    bool _isDebug { false };
};
