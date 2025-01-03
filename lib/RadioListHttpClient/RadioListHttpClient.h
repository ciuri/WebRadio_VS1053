#ifndef RADIOLISTHTTPCLIENT_H
#define RADIOLISTHTTPCLIENT_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DTOs.h>
#include <DeviceConfiguration.h>

using namespace std;

class RadioListHttpClient
{
private:
    DeviceConfiguration *_config;
    HTTPClient httpClient;
    int tagsPerPage = 6;
    int tagsPageIndex = 0;
    int countriesPerPage = 6;
    int countriesPageIndex = 0;
    int stationsPerPage = 6;
    int stationsPageIndex = 0;
    String selectedTag;
    String selectedCountry;    

public:
    RadioListHttpClient(DeviceConfiguration *config);
    vector<TagDTO> GetTags();
    vector<CountryDTO> GetCountries();
    vector<RadioStationDTO> GetRadioURLsByCountry();
    vector<RadioStationDTO> GetRadioURLsByTag();
    vector<RadioStationDTO> GetRadioURLs(UIState state);
    void ResetStationsPageIndex();
    void SetCountry(String country);
    void SetTag(String tag);
    void SetNextStationsPage();
    void SetPrevStationsPage();
    void SetNextCountriesPage();
    void SetPrevCountriesPage();
    void SetNextTagsPage();
    void SetPrevTagsPage();
    int GetPageStartIndex();
};

RadioListHttpClient::RadioListHttpClient(DeviceConfiguration *config)
{
    _config = config;   
}

void RadioListHttpClient::ResetStationsPageIndex()
{
    stationsPageIndex = 0;
}

int RadioListHttpClient::GetPageStartIndex()
{
    return stationsPageIndex * stationsPerPage;
}

vector<RadioStationDTO> RadioListHttpClient::GetRadioURLs(UIState state)
{
    Serial.println(state);
    switch (state)
    {
    case SELECT_TAG:
        return GetRadioURLsByTag();
        break;
    case SELECT_COUNTRY:
        return GetRadioURLsByCountry();
        break;
    default:
        return GetRadioURLsByCountry();
    }
}

String urlEncode(const char *msg) {
  const char *hex = "0123456789ABCDEF";
  String encodedMsg = "";

  while (*msg != '\0') {
    if (
        ('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') || ('0' <= *msg && *msg <= '9') || 
        *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~' || *msg=='/' || *msg == ':' || *msg== '=' || *msg =='?' || *msg=='&') {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[(unsigned char)*msg >> 4];
      encodedMsg += hex[*msg & 0xf];
    }
    msg++;
  }
  return encodedMsg;
}

String urlEncode(String msg) {
  return urlEncode(msg.c_str());
}


vector<TagDTO> RadioListHttpClient::GetTags()
{
    vector<TagDTO> outList;
    DynamicJsonDocument jsonDoc(8072);
    char urlText[300];
    sprintf(urlText, "https://%s/json/tags?order=stationcount&limit=%i&reverse=true&offset=%i", _config->serverName.c_str(), tagsPerPage, tagsPerPage * tagsPageIndex);
    Serial.println(urlEncode(urlText));
    httpClient.begin(urlEncode(urlText));
    int result = httpClient.GET();
    // String outString = httpClient.getString();
    deserializeJson(jsonDoc, httpClient.getStream());
    httpClient.end();
    for (auto item : jsonDoc.as<JsonArray>())
    {
        const char *name = item["name"];
        int stationsCount = item["stationcount"];
        TagDTO tagDto;
        tagDto.name = name;
        tagDto.count = stationsCount;
        outList.push_back(tagDto);
    }
    jsonDoc.clear();
    return outList;
}

vector<CountryDTO> RadioListHttpClient::GetCountries()
{
    vector<CountryDTO> outList;
    DynamicJsonDocument jsonDoc(8072);
    char urlText[400];
    sprintf(urlText, "https://%s/json/countries?order=stationcount&limit=%i&reverse=true&offset=%i", _config->serverName.c_str(), countriesPerPage, countriesPerPage * countriesPageIndex);
    httpClient.begin(urlEncode(urlText));
    httpClient.GET();
    deserializeJson(jsonDoc, httpClient.getStream());
    httpClient.end();
    for (auto item : jsonDoc.as<JsonArray>())
    {
        const char *name = item["name"];
        const char *code = item["iso_3166_1"];
        int stationsCount = item["stationcount"];
        CountryDTO c;
        c.name = name;
        c.code = code;
        c.count = stationsCount;
        outList.push_back(c);
    }
    jsonDoc.clear();
    return outList;
}
vector<RadioStationDTO> RadioListHttpClient::GetRadioURLsByCountry()
{
    vector<RadioStationDTO> outList;
    DynamicJsonDocument jsonDoc(30000);
    char urlText[300];
    sprintf(urlText, "http://%s/json/stations/bycountrycodeexact/%s?limit=%i&order=clickcount&reverse=true&hidebroken=true&offset=%i", _config->serverName.c_str(), selectedCountry, stationsPerPage, stationsPerPage * stationsPageIndex);
    httpClient.begin(urlEncode(urlText));
    int result = httpClient.GET();
    // String outString = httpClient.getString();
    deserializeJson(jsonDoc, httpClient.getStream());
    httpClient.end();
    for (auto item : jsonDoc.as<JsonArray>())
    {
        const char *url = item["url_resolved"];
        std::string url_s = std::string(url);
        if (url_s.find(".m3u") != std::string::npos)
            continue;
        const char *name = item["name"];
        const char *country = item["country"];
        int bitrate = item["bitrate"];
        RadioStationDTO rs;
        rs.Name = name;
        rs.Bitrate = bitrate;
        rs.Url = url;
        rs.Country = country;
        outList.push_back(rs);
        Serial.println(rs.Name);
    }
    jsonDoc.clear();
    return outList;
}

vector<RadioStationDTO> RadioListHttpClient::GetRadioURLsByTag()
{
    vector<RadioStationDTO> outList;
    DynamicJsonDocument jsonDoc(30000);
    char urlText[300];
    sprintf(urlText, "http://%s/json/stations/bytag/%s?limit=%i&order=clickcount&reverse=true&hidebroken=true&offset=%i", _config->serverName.c_str(), selectedTag, stationsPerPage, stationsPerPage * stationsPageIndex);
    Serial.println(urlEncode(urlText));
    httpClient.begin(urlEncode(urlText));
    int result = httpClient.GET();
    // String outString = httpClient.getString();
    deserializeJson(jsonDoc, httpClient.getStream());
    httpClient.end();
    for (auto item : jsonDoc.as<JsonArray>())
    {
        const char *url = item["url_resolved"];
        std::string url_s = std::string(url);
        if (url_s.find(".m3u") != std::string::npos)
            continue;
        const char *name = item["name"];
        const char *country = item["country"];
        int bitrate = item["bitrate"];
        RadioStationDTO rs;
        rs.Name = name;
        rs.Bitrate = bitrate;
        rs.Url = url;
        rs.Country = country;
        outList.push_back(rs);
        Serial.println(rs.Name);
    }    
    jsonDoc.clear();
    return outList;
}

void RadioListHttpClient::SetCountry(String country)
{
    selectedCountry = country;
}

void RadioListHttpClient::SetTag(String tag)
{
    selectedTag = tag;
}

void RadioListHttpClient::SetNextStationsPage()
{
    stationsPageIndex++;
}

void RadioListHttpClient::SetPrevStationsPage()
{
    if (stationsPageIndex > 0)
        stationsPageIndex--;
}

void RadioListHttpClient::SetNextCountriesPage()
{
    countriesPageIndex++;
}

void RadioListHttpClient::SetPrevCountriesPage()
{
    if (countriesPageIndex > 0)
        countriesPageIndex--;
}
void RadioListHttpClient::SetNextTagsPage()
{
    tagsPageIndex++;
}

void RadioListHttpClient::SetPrevTagsPage()
{
    if (tagsPageIndex > 0)
        tagsPageIndex--;
}
#endif