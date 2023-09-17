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
    void HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display);
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

void StationsListState::HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display)
{
    display->setFont(u8g2_font_NokiaSmallPlain_tf);
    display->firstPage();
    do
    {
        for (int i = 0; i < radioStations.size(); i++)
        {
            display->setCursor(0, i * 10 + 8);
            if (i != currentStationIndex)
            {
             display->printf(radioStations[i].Name.c_str());   
            }
            else
            {
                display->drawButtonUTF8(2, i*10+8, U8G2_BTN_BW1, 120, 1, 0, radioStations[currentStationIndex].Name.c_str());
            }
        }

        /*  display->setCursor(0, 10);
          display->printf(radioStations[currentStationIndex].Name.c_str());
          display->setCursor(0, 20);
          display->printf("Bitrate: %i ", radioStations[currentStationIndex].Bitrate);
          display->setCursor(0, 30);
          display->printf("%i", radioListClient.GetPageStartIndex() + currentStationIndex + 1);*/
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