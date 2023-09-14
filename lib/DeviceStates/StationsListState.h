#ifndef COUNTRIESLISTSTATE_H
#define COUNTRIESLISTSTATE_H
#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <RadioListHttpClient.h>

class StationsListState
{
private:
    UIState _lastState;

public:
    UIState EnterState(UIState lastState, String country, String tag, SelectBy by);
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
    UIState HandleBack();
};

void StationsListState::GetRadioUrlsPage()
{
    radioStations = radioListClient.GetRadioURLs();
}

UIState StationsListState::EnterState(UIState lastState, String country, String tag, SelectBy by)
{
    _lastState = lastState;
    currentStationIndex = 0;
    radioListClient.ResetStationsPageIndex();
    radioListClient.SetCountry(country);
    radioListClient.SetTag(tag);
    radioListClient.SetSelectBy(by);
    radioStations = radioListClient.GetRadioURLs();

    return SELECT_STATION;
}

void StationsListState::HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display)
{
    display->setFont(u8g2_font_ncenB08_tr);
    display->firstPage();
    do
    {
        display->setCursor(0, 20);
        display->printf(radioStations[currentStationIndex].Name.c_str());
        display->setCursor(0, 40);
        display->printf("Bitrate: %i ", radioStations[currentStationIndex].Bitrate);
        display->setCursor(0, 60);
        display->printf("%i", radioListClient.GetPageStartIndex() + currentStationIndex + 1);
    } while (display->nextPage());
}

UIState StationsListState::HandleBack()
{
    return _lastState;
}

void StationsListState::HandleUp()
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

void StationsListState::HandleDown()
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

RadioStationDTO StationsListState::HandleEnter()
{
    return radioStations[currentStationIndex];
}

void StationsListState::HandleRight()
{
    radioListClient.SetNextStationsPage();
    GetRadioUrlsPage();
    currentStationIndex = 0;
}

void StationsListState::HandleLeft()
{
    radioListClient.SetPrevStationsPage();
    GetRadioUrlsPage();
    currentStationIndex = 0;
}

#endif