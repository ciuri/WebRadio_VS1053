#ifndef SELECT_FAVORITES_STATE_H
#define SELECT_FAVORITES_STATE_H
#define LIST_START_OFFSET 20

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <RadioListHttpClient.h>
#include <DeviceConfiguration.h>
#include <PlayingState.h>

class SelectFavoritesState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    PlayingState *_playingState;
    DeviceConfiguration* _config;

public:
    SelectFavoritesState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, PlayingState *playingState, DeviceConfiguration* config);
    UIState EnterState(UIState lastState);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    vector<RadioStationDTO> radioStations;
    int currentStationIndex = 0;   
    bool HandleEnter();
    bool HandleBack();
};

SelectFavoritesState::SelectFavoritesState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, PlayingState *playingState, DeviceConfiguration* config)
{
    _currentState = currentState;
    _display = display;
    _playingState = playingState;
    _config = config;
}

UIState SelectFavoritesState::EnterState(UIState lastState)
{
    _lastState = lastState;
    currentStationIndex = 0;  
    _config->LoadConfiguration();
    radioStations = _config->favorites;

    for(auto &i: radioStations)
    {
        Serial.println(i.Name);
    }

    return SELECT_FAVORITES;
}

void SelectFavoritesState::HandleLoop()
{
    if (*_currentState != SELECT_FAVORITES)
        return;

     _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        _display->setCursor(35, 8);
        _display->printf("Favorites:");

        int itemsPerPage = 5;
        int dataCount = radioStations.size();
        int page = currentStationIndex / itemsPerPage;

        for (int i = itemsPerPage * page; i < itemsPerPage * page + itemsPerPage && i < dataCount; i++)
        {
            char rowText[120];
            sprintf(rowText, "%s", radioStations[i].Name.c_str());

            _display->setCursor(0, i % itemsPerPage * 10 + LIST_START_OFFSET);
            if (i != currentStationIndex)
            {
                _display->printf(rowText);
            }
            else
            {
                _display->drawButtonUTF8(2, i % itemsPerPage * 10 + LIST_START_OFFSET, U8G2_BTN_BW1, 120, 1, 0, rowText);
            }
        }
    } while (_display->nextPage());
}

bool SelectFavoritesState::HandleBack()
{
    if (*_currentState != SELECT_FAVORITES)
        return false;

    *_currentState = _lastState;
    return true;
}

void SelectFavoritesState::HandleUp()
{
    if (*_currentState != SELECT_FAVORITES)
        return;

    if (currentStationIndex > 0)
    {
        currentStationIndex--;
        Serial.printf("\nSelected: %s, [Bitrate: %i]", radioStations[currentStationIndex].Name.c_str(), radioStations[currentStationIndex].Bitrate);
    }   
}

void SelectFavoritesState::HandleDown()
{
    if (*_currentState != SELECT_FAVORITES)
        return;

    if (currentStationIndex < radioStations.size() - 1)
    {
        currentStationIndex++;
        Serial.printf("\nSelected: %s, [Bitrate: %i]", radioStations[currentStationIndex].Name.c_str(), radioStations[currentStationIndex].Bitrate);
    }   
}

bool SelectFavoritesState::HandleEnter()
{
    if (*_currentState != SELECT_FAVORITES)
        return false;

    *_currentState = _playingState->EnterState(SELECT_FAVORITES, radioStations[currentStationIndex]);
    return true;
}

void SelectFavoritesState::HandleRight()
{
    if (*_currentState != SELECT_FAVORITES)
        return;   
}

void SelectFavoritesState::HandleLeft()
{
    if (*_currentState != SELECT_FAVORITES)
        return;
}

#endif