#pragma once
#include <string>
#include <memory>
#include <boost/property_tree/ptree.hpp>

// Settings should look like:
//{
//"settings": {
//  "serverIP": "127.0.0.1",
//  "serverPort": 0000,
//  "storageDirectory": "~/storage"
//  }
//}


class TCPIPToolSettingsParser
{
public:
    static std::shared_ptr<TCPIPToolSettingsParser> getInstance();

    std::string getServerIP() const;
    int getServerPort() const;
    std::string getStorageDirectory() const;
    void setSettingsPath(std::string settingsPath);

private:
    TCPIPToolSettingsParser();
    boost::property_tree::ptree tree;
    std::string settingsPath = "settings.json";
};