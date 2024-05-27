#include "TCPIPToolSettingsParser.hpp"
#include <boost/property_tree/json_parser.hpp>

TCPIPToolSettingsParser::TCPIPToolSettingsParser()
{
    boost::property_tree::read_json(settingsPath, tree);
}

std::shared_ptr<TCPIPToolSettingsParser> TCPIPToolSettingsParser::getInstance()
{
    static std::shared_ptr<TCPIPToolSettingsParser> instance{new TCPIPToolSettingsParser{}};
    return instance;
}

std::string TCPIPToolSettingsParser::getServerIP() const
{
    return tree.get<std::string>("settings.serverIP");
}

int TCPIPToolSettingsParser::getServerPort() const
{
    return tree.get<int>("settings.serverPort");
}

std::string TCPIPToolSettingsParser::getStorageDirectory() const
{
    return tree.get<std::string>("settings.storageDirectory");
}
