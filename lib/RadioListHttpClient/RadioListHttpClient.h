#ifndef RADIOLISTHTTPCLIENT_H
#define RADIOLISTHTTPCLIENT_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DTOs.h>

using namespace std;

class RadioListHttpClient
{
    private:
    HTTPClient httpClient;
    int tagsPerPage = 5;
    int tagsPageIndex =0;
    int countriesPerPage =5;
    int countriesPageIndex = 0;
    int stationsPerPage = 10;
    int stationsPageIndex =0;
    String selectedTag;
    String selectedCountry;
    SelectBy selectBy;

    public:
    vector<TagDTO> GetTags();
    vector<CountryDTO> GetCountries();
    vector<RadioStationDTO> GetRadioURLsByCountry();
    vector<RadioStationDTO> GetRadioURLsByTag();
    vector<RadioStationDTO> GetRadioURLs();
    void ResetStationsPageIndex();
    void SetCountry(String country);
    void SetTag(String tag);
    void SetNextStationsPage();
    void SetPrevStationsPage();
    void SetNextCountriesPage();
    void SetPrevCountriesPage();
    void SetNextTagsPage();
    void SetPrevTagsPage();
    void SetSelectBy(SelectBy sb);
    int GetPageStartIndex();
};

void RadioListHttpClient::ResetStationsPageIndex()
{
    stationsPageIndex=0;
}

int RadioListHttpClient::GetPageStartIndex()
{
    return stationsPageIndex*stationsPerPage;
}

void RadioListHttpClient::SetSelectBy(SelectBy sb)
{
    selectBy = sb;
}

vector<RadioStationDTO> RadioListHttpClient::GetRadioURLs()
{
    switch(selectBy)
    {
    case TAG:
        return GetRadioURLsByTag();
        break;
    case COUNTRY:
        return GetRadioURLsByCountry();
        break;
    }   
    return GetRadioURLsByCountry(); //default
}
vector<TagDTO> RadioListHttpClient::GetTags()
{
    vector<TagDTO> outList;
    DynamicJsonDocument jsonDoc(8072);
    char urlText[300];
    sprintf(urlText,"https://at1.api.radio-browser.info/json/tags?order=stationcount&limit=%i&reverse=true&offset=%i",tagsPerPage, tagsPerPage*tagsPageIndex);
    httpClient.begin(urlText);
    int result = httpClient.GET();
    //String outString = httpClient.getString();
    deserializeJson(jsonDoc,httpClient.getStream());
    httpClient.end();
    for(auto item : jsonDoc.as<JsonArray>())
    {
        const char* name = item["name"];
        int stationsCount = item["stationcount"];
        TagDTO tagDto;
        tagDto.name = name;
        tagDto.count = stationsCount;
        outList.push_back(tagDto);
    }    
    return outList;
}

vector<CountryDTO> RadioListHttpClient::GetCountries()
{
    vector<CountryDTO> outList;
    DynamicJsonDocument jsonDoc(8072);
    char urlText[300];
    sprintf(urlText,"https://at1.api.radio-browser.info/json/countries?order=stationcount&limit=%i&reverse=true&offset=%i", countriesPerPage, countriesPerPage*countriesPageIndex);
    httpClient.begin(urlText);
    int result = httpClient.GET();
    //String outString = httpClient.getString();
    deserializeJson(jsonDoc,httpClient.getStream());
    httpClient.end();
    for(auto item : jsonDoc.as<JsonArray>())
    {
        const char* name = item["name"];
        const char* code = item["iso_3166_1"];
        int stationsCount = item["stationcount"];
        CountryDTO c;
        c.name= name;
        c.code = code;
        c.count = stationsCount;
        outList.push_back(c);
    }    
    return outList;
}
vector<RadioStationDTO> RadioListHttpClient::GetRadioURLsByCountry()
{
    vector<RadioStationDTO> outList;
    DynamicJsonDocument jsonDoc(30000);
    char urlText[300];
    sprintf(urlText,"http://at1.api.radio-browser.info/json/stations/bycountrycodeexact/%s?limit=%i&order=clickcount&reverse=true&hidebroken=true&offset=%i", selectedCountry, stationsPerPage, stationsPerPage*stationsPageIndex);
    httpClient.begin(urlText);
    int result = httpClient.GET();
    //String outString = httpClient.getString();
    deserializeJson(jsonDoc,httpClient.getStream());
    httpClient.end();
     for(auto item : jsonDoc.as<JsonArray>())
    {
        const char* url = item["url_resolved"];
        std::string url_s = std::string(url);
        if(url_s.find(".m3u") != std::string::npos)
            continue;
        const char* name = item["name"];
        int bitrate = item["bitrate"];
        RadioStationDTO rs;
        rs.Name = name;
        rs.Bitrate = bitrate;
        rs.Url = url;    
        outList.push_back(rs);
        Serial.println(rs.Name);
    }   
    return outList;
}

vector<RadioStationDTO> RadioListHttpClient::GetRadioURLsByTag()
{
    vector<RadioStationDTO> outList;
    DynamicJsonDocument jsonDoc(30000);
    char urlText[300];
    sprintf(urlText,"http://at1.api.radio-browser.info/json/stations/bytag/%s?limit=%i&order=clickcount&reverse=true&hidebroken=true&offset=%i", selectedTag, stationsPerPage, stationsPerPage*stationsPageIndex);
    httpClient.begin(urlText);
    int result = httpClient.GET();
    //String outString = httpClient.getString();
    deserializeJson(jsonDoc,httpClient.getStream());
    httpClient.end();
    for(auto item : jsonDoc.as<JsonArray>())
    {
        const char* url = item["url_resolved"];
        std::string url_s = std::string(url);
        if(url_s.find(".m3u") != std::string::npos)
            continue;
        const char* name = item["name"];
        int bitrate = item["bitrate"];
        RadioStationDTO rs;
        rs.Name = name;
        rs.Bitrate = bitrate;
        rs.Url = url;    
        outList.push_back(rs);
        Serial.println(rs.Name);
    }    
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
    if(stationsPageIndex>0)
        stationsPageIndex--;
}

void RadioListHttpClient::SetNextCountriesPage()
{
    countriesPageIndex++;
}

void RadioListHttpClient::SetPrevCountriesPage()
{
    if(countriesPageIndex>0)
        countriesPageIndex--;
}
void RadioListHttpClient::SetNextTagsPage()
{
    tagsPageIndex++;
}

void RadioListHttpClient::SetPrevTagsPage()
{
    if(tagsPageIndex>0)
        tagsPageIndex--;
}
#endif