#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#define CONFIGURATION_FILE_PATH "/configuration.json"

#include <ArduinoJson.h>
#include <DTOs.h>
#include <SPIFFSWrapper.h>

using namespace std;

struct DeviceConfiguration
{
    String serverName;
    String wifiName;
    String wifiPassword;
    vector<RadioStationDTO> favorites;

    String SerializeConfguration()
    {
        DynamicJsonDocument configJson(4096);
        configJson["serverName"] = serverName;
        configJson["wifiName"] = wifiName;
        configJson["wifiPassword"] = wifiPassword;

        JsonArray favoritesArray = configJson.createNestedArray("favorites");

        for (const auto &station : favorites)
        {
            JsonObject stationObject = favoritesArray.createNestedObject();
            stationObject["name"] = station.Name;
            stationObject["url"] = station.Url;
            stationObject["bitrate"] = station.Bitrate;
            stationObject["country"] = station.Country;
            stationObject["radiofrequency"] = station.RadioFrequency;
        }

        String jsonString;
        serializeJson(configJson, jsonString);
        return jsonString;
    }

    void DeserializeDeviceConfiguration(const char *jsonString)
    {
        const size_t capacity = JSON_OBJECT_SIZE(4) + 80000;
        DynamicJsonDocument jsonDocument(capacity);

        DeserializationError error = deserializeJson(jsonDocument, jsonString);
        if (error)
        {
            Serial.print("Json serialization errror: ");
            Serial.println(error.c_str());
            return;
        }

        serverName = jsonDocument["serverName"].as<String>();
        wifiName = jsonDocument["wifiName"].as<String>();
        wifiPassword = jsonDocument["wifiPassword"].as<String>();
        favorites.clear();
        JsonArray favoritesArray = jsonDocument["favorites"];
        for (const auto &station : favoritesArray)
        {
            RadioStationDTO favoriteStation;
            favoriteStation.Bitrate = station["bitrate"];
            favoriteStation.Country = station["country"].as<String>();
            favoriteStation.Name = station["name"].as<String>();
            favoriteStation.RadioFrequency = station["radiofrequency"].as<String>();
            favoriteStation.Url = station["url"].as<String>();
            favorites.push_back(favoriteStation);
        }        
    }

    void SaveConfiguration()
    {
        String configJson = SerializeConfguration();
        Serial.println("Saving configuration:");
        Serial.println(configJson);
        writeFile(SPIFFS, CONFIGURATION_FILE_PATH, configJson.c_str());
    }

    void LoadConfiguration()
    {
        String s = readFile(SPIFFS, CONFIGURATION_FILE_PATH);
        Serial.println("Load configuration:");
        Serial.println(s);
        DeserializeDeviceConfiguration(s.c_str());
    }
};


#endif