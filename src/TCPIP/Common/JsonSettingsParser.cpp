#include "JsonSettingsParser.hpp"
#include <boost/property_tree/json_parser.hpp>

JsonSettingsParser::JsonSettingsParser()
{
    ;
}

std::shared_ptr<JsonSettingsParser> JsonSettingsParser::getInstance()
{
    static std::shared_ptr<JsonSettingsParser> instance{new JsonSettingsParser{}};
    return instance;
}

std::string JsonSettingsParser::getServerIP() const
{
    return tree.get<std::string>("settings.serverIP");
}

int JsonSettingsParser::getServerPort() const
{
    return tree.get<int>("settings.serverPort");
}

std::string JsonSettingsParser::getStorageDirectory() const
{
    return tree.get<std::string>("settings.storageDirectory");
}

void JsonSettingsParser::setSettingsPath(std::string settingsPath)
{
    this->settingsPath = std::move(settingsPath);
    boost::property_tree::read_json(this->settingsPath, tree);
}
