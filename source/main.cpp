#include "ConfigHolder.h"
#include "CinemaServer.h"
#include "Logger.h"

int main(int argc, char** argv)
{
    ConfigHolder &ch = ConfigHolder::getInstance();
    if (!ch.init(argc, argv))
        return 1;

    Logger::getInstance().setIsDebug(ch.getIsDebugLogs());

    CinemaServer cs(ch.getInstance().getHttpPort(), ch.getInstance().getHttpFolderPath());
    cs.start();

    return 0;
}
