#include "JsonSettingsParser.hpp"
#include <boost/property_tree/json_parser.hpp>

TCPIP::JsonSettingsParser::JsonSettingsParser()
{
    ;
}

std::shared_ptr<TCPIP::JsonSettingsParser> TCPIP::JsonSettingsParser::getInstance()
{
    static std::shared_ptr<JsonSettingsParser> instance{new JsonSettingsParser{}};
    return instance;
}

std::string TCPIP::JsonSettingsParser::getServerIP() const
{
    return tree.get<std::string>("settings.serverIP");
}

int TCPIP::JsonSettingsParser::getServerPort() const
{
    return tree.get<int>("settings.serverPort");
}

std::string TCPIP::JsonSettingsParser::getStorageDirectory() const
{
    return tree.get<std::string>("settings.storageDirectory");
}

void TCPIP::JsonSettingsParser::setSettingsPath(std::string settingsPath)
{
    this->settingsPath = std::move(settingsPath);
    boost::property_tree::read_json(this->settingsPath, tree);
}

int TCPIP::JsonSettingsParser::loadInidicatorRefreshRate() const
{
    return tree.get<int>("settings.loadIndicatorRefreshInMs");
}

bool TCPIP::JsonSettingsParser::loadIndicatorEnabled() const
{
    return tree.get<bool>("settings.showIndicator");
}
