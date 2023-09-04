#ifndef STATIONSLISTBYCOUNTRYSTATE_H
#define STATIONSLISTBYCOUNTRYSTATE_H
#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <RadioListHttpClient.h>

class StationsListByCountryState
{
private:
    UIState _lastState;

public:
    UIState EnterState(UIState lastState);
    void HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display);
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetRadioUrlsPage();
    vector<RadioStationDTO> radioStations;
    int currentStationIndex = 0;
    RadioListHttpClient radioListClient;

    RadioStationDTO HandleEnter();
    UIState HandleBack(UIState uiLastState);
};

void StationsListByCountryState::GetRadioUrlsPage()
{    
    radioStations = radioListClient.GetRadioURLsByCountry();
}

UIState StationsListByCountryState::EnterState(UIState lastState)
{
    lastState = _lastState;
   
    radioListClient.SetCountry("PL");
    radioStations = radioListClient.GetRadioURLsByCountry();

    return SELECT_BY_COUNTRY;
}

void StationsListByCountryState::HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display)
{
    display->setFont(u8g2_font_ncenB08_tr);
    display->firstPage();
    do
    {
        display->setCursor(0, 20);
        display->printf(radioStations[currentStationIndex].Name.c_str());
        display->setCursor(0, 40);
        display->printf("Bitrate: %i ", radioStations[currentStationIndex].Bitrate);
    } while (display->nextPage());
}

UIState StationsListByCountryState::HandleBack(UIState uiLastState)
{
    return _lastState;
}

void StationsListByCountryState::HandleUp()
{
    if (currentStationIndex > 0)
    {
        currentStationIndex--;
        Serial.printf("\nSelected: %s, [Bitrate: %i]", radioStations[currentStationIndex].Name.c_str(), radioStations[currentStationIndex].Bitrate);
    }
    else
    {
        radioListClient.SetPrevStationsPage();
        GetRadioUrlsPage();
        currentStationIndex = radioStations.size() - 1;
    }
}

void StationsListByCountryState::HandleDown()
{
    if (currentStationIndex < radioStations.size() - 1)
    {
        currentStationIndex++;
        Serial.printf("\nSelected: %s, [Bitrate: %i]", radioStations[currentStationIndex].Name.c_str(), radioStations[currentStationIndex].Bitrate);
    }
    else
    {
        radioListClient.SetNextStationsPage();
        GetRadioUrlsPage();
        currentStationIndex = 0;
    }
}

RadioStationDTO StationsListByCountryState::HandleEnter()
{
    return radioStations[currentStationIndex];
}

void StationsListByCountryState::HandleRight()
{
    radioListClient.SetNextStationsPage();
    GetRadioUrlsPage();
    currentStationIndex = 0;
}

void StationsListByCountryState::HandleLeft()
{
    radioListClient.SetPrevStationsPage();
    GetRadioUrlsPage();
    currentStationIndex = 0;
}

#endif