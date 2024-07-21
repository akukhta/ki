#pragma once
#include <string>
#include <memory>
#include <boost/property_tree/ptree.hpp>

// Settings should look like:
//{
//"settings": {
//  "serverIP": "127.0.0.1",
//  "serverPort": 0000,
//  "storageDirectory": "~/storage",
//  "showIndicator" : true,
//  "loadIndicatorRefreshInMs" : 50,
//  "encryption":
//  {
//      "key" : "someKey",
//      "IV" : "someIV"
//  }
//  }
//}

namespace TCPIP {
    /// Json Settings parser
    class JsonSettingsParser {
    public:
        static std::shared_ptr<JsonSettingsParser> getInstance();

        [[nodiscard]] std::string getServerIP() const;

        [[nodiscard]] int getServerPort() const;

        [[nodiscard]] std::string getStorageDirectory() const;

        void setSettingsPath(std::string settingsPath);

        [[nodiscard]] int loadInidicatorRefreshRate() const;

        [[nodiscard]] bool loadIndicatorEnabled() const;

        [[nodiscard]] std::vector<char> getEncryptionKey() const;
        [[nodiscard]] std::vector<char> getEncryptionIV() const;
    private:
        JsonSettingsParser();

        boost::property_tree::ptree tree;
        std::string settingsPath = "settings.json";
    };
}