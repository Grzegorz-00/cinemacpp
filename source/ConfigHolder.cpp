#include "ConfigHolder.h"

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;

bool ConfigHolder::parseArgs(int argc, char** argv)
{
    bool isOk { false };
    po::options_description description("Usage:");

    description.add_options()("help,h", "Print usage message")
                             ("port,p", po::value(&_httpPort), "Http port")
                             ("httpFolderPath,f", po::value(&_httpFolderPath), "Http webpage folder path")
                             ("debug,d", po::bool_switch()->default_value(false), "Enable debug logs");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << description << std::endl;
    }

    if (vm.count("port"))
    {
        _httpPort = vm["port"].as<uint16_t>();
        isOk = true;
    }
    else
    {
        std::cout << "Missing port parameter" << std::endl;
    }

    if (vm.count("httpFolderPath"))
    {
       _httpFolderPath = vm["httpFolderPath"].as<std::string>();
    }

    if (vm.count("debug"))
    {
        _isDebugLogs = true;
    }

    return isOk;
}
