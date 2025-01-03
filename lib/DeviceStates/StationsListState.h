#ifndef COUNTRIESLISTSTATE_H
#define COUNTRIESLISTSTATE_H
#include <DeviceStateBase.h>
#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <RadioListHttpClient.h>


class StationsListState : public DeviceStateBase
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    PlayingState *_playingState;
    RadioListHttpClient* _radioListClient;

public:
    StationsListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, PlayingState *playingState, RadioListHttpClient* radioListClient);
    UIState EnterState(UIState lastState, String country, String tag);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetRadioUrlsPage();
    vector<RadioStationDTO> radioStations;
    int currentStationIndex = 0;
   

    bool HandleEnter();
    bool HandleBack();
};

StationsListState::StationsListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, PlayingState *playingState, RadioListHttpClient* radioListClient)
{
    _currentState = currentState;
    _display = display;
    _playingState = playingState;
    _radioListClient = radioListClient;
}

void StationsListState::GetRadioUrlsPage()
{
    radioStations = _radioListClient->GetRadioURLs(_lastState);
}

UIState StationsListState::EnterState(UIState lastState, String country, String tag)
{
    _lastState = lastState;
    currentStationIndex = 0;
    _radioListClient->ResetStationsPageIndex();
    _radioListClient->SetCountry(country);
    _radioListClient->SetTag(tag);   
    radioStations = _radioListClient->GetRadioURLs(_lastState);

    return SELECT_STATION;
}

void StationsListState::HandleLoop()
{
    if (*_currentState != SELECT_STATION)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);  
    _display->firstPage();
    do
    {
        for (int i = 0; i < radioStations.size(); i++)
        {
            _display->setCursor(0, i * 10 + 8);
            if (i != currentStationIndex)
            {
                _display->printf(radioStations[i].Name.c_str());
            }
            else
            {
                _display->drawButtonUTF8(2, i * 10 + 8, U8G2_BTN_BW1, 120, 1, 0, radioStations[currentStationIndex].Name.c_str());
            }
        }
    } while (_display->nextPage());
}

bool StationsListState::HandleBack()
{
    if (*_currentState != SELECT_STATION)
        return false;

    *_currentState = _lastState;
    return true;
}

void StationsListState::HandleUp()
{
    if (*_currentState != SELECT_STATION)
        return;

    if (currentStationIndex > 0)
    {
        currentStationIndex--;
        Serial.printf("\nSelected: %s, [Bitrate: %i]", radioStations[currentStationIndex].Name.c_str(), radioStations[currentStationIndex].Bitrate);
    }
    else
    {
        _radioListClient->SetPrevStationsPage();
        GetRadioUrlsPage();
        currentStationIndex = radioStations.size() - 1;
    }
}

void StationsListState::HandleDown()
{
    if (*_currentState != SELECT_STATION)
        return;

    if (currentStationIndex < radioStations.size() - 1)
    {
        currentStationIndex++;
        Serial.printf("\nSelected: %s, [Bitrate: %i]", radioStations[currentStationIndex].Name.c_str(), radioStations[currentStationIndex].Bitrate);
    }
    else
    {

        _radioListClient->SetNextStationsPage();
        GetRadioUrlsPage();
        currentStationIndex = 0;
    }
}

bool StationsListState::HandleEnter()
{
    if (*_currentState != SELECT_STATION)
        return false;

    *_currentState = _playingState->EnterState(SELECT_STATION, radioStations[currentStationIndex]);
    return true;
}

void StationsListState::HandleRight()
{
    if (*_currentState != SELECT_STATION)
        return;

    _radioListClient->SetNextStationsPage();
    GetRadioUrlsPage();
    currentStationIndex = 0;
}

void StationsListState::HandleLeft()
{
    if (*_currentState != SELECT_STATION)
        return;

    _radioListClient->SetPrevStationsPage();
    GetRadioUrlsPage();
    currentStationIndex = 0;
}

#endif