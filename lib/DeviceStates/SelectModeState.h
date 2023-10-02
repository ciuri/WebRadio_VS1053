#ifndef SELECTMODESTATE_H
#define SELECTMODESTATE_H

#define LIST_START_OFFSET 20

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <SelectSettingsState.h>
#include <SelectFavoritesState.h>

class SelectModeState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    CountriesListState *_countriesListState;
    TagsListState *_tagsListState;
    SelectSettingsState *_selectSettingsState;
    SelectFavoritesState *_selectFavoritesState;    

public:
    SelectModeState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, CountriesListState *countriesListState, TagsListState *tagsListState, SelectSettingsState *selectSettingsState, SelectFavoritesState *selectFavoritesState);
    UIState EnterState(UIState lastState);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetTagsPage();
    vector<NamedModeDTO> modes;
    int currentIndex = 0;

    bool HandleEnter();
    bool HandleBack();
    void GoSleep();
};

SelectModeState::SelectModeState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, CountriesListState *countriesListState, TagsListState *tagsListState, SelectSettingsState *selectSettingsState, SelectFavoritesState *selectFavoritesState)
{
    _currentState = currentState;
    _display = display;
    _countriesListState = countriesListState;
    _tagsListState = tagsListState;
    _selectSettingsState = selectSettingsState;
    _selectFavoritesState = selectFavoritesState;
}

UIState SelectModeState::EnterState(UIState lastState)
{
    _lastState = lastState;
    modes.clear();

    NamedModeDTO favorites;
    favorites.name = "Favorites";
    favorites.state = SELECT_FAVORITES;

    NamedModeDTO byCountry;
    byCountry.name = "Select by country";
    byCountry.state = SELECT_COUNTRY;

    NamedModeDTO byTag;
    byTag.name = "Select by tag";
    byTag.state = SELECT_TAG;

    NamedModeDTO settings;
    settings.name = "Settings";
    settings.state = SELECT_SETTINGS;

    NamedModeDTO sleep;
    sleep.name = "Turn off";
    sleep.state = SLEEP;

    if (WiFi.status() == WL_CONNECTED)
    {
        modes.push_back(favorites);
        modes.push_back(byCountry);
        modes.push_back(byTag);
    }
    modes.push_back(settings);
    modes.push_back(sleep);
    return MODE_SELECT;
}

void SelectModeState::HandleLoop()
{
    if (*_currentState != MODE_SELECT)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        _display->setCursor(40, 8);
        _display->printf("MAIN MENU");

        for (int i = 0; i < modes.size(); i++)
        {
            char rowText[120];
            sprintf(rowText, "%s", modes[i].name.c_str());
            _display->setCursor(0, i * 10 + LIST_START_OFFSET);
            if (i != currentIndex)
            {
                _display->printf(rowText);
            }
            else
            {
                _display->drawButtonUTF8(2, i * 10 + LIST_START_OFFSET, U8G2_BTN_BW1, 120, 1, 0, rowText);
            }
        }
    } while (_display->nextPage());
}

bool SelectModeState::HandleBack()
{
    if (*_currentState != MODE_SELECT)
        return false;

  
    *_currentState = _lastState;
    return true;
}

void SelectModeState::HandleUp()
{
    if (*_currentState != MODE_SELECT)
        return;

    if (currentIndex > 0)
    {
        currentIndex--;
    }
}

void SelectModeState::HandleDown()
{
    if (*_currentState != MODE_SELECT)
        return;

    if (currentIndex < modes.size() - 1)
    {
        currentIndex++;
    }
}

bool SelectModeState::HandleEnter()
{
    if (*_currentState != MODE_SELECT)
        return false;

    switch (modes[currentIndex].state)
    {
    case SELECT_FAVORITES:
        *_currentState = _selectFavoritesState->EnterState(MODE_SELECT);
        break;
    case SELECT_TAG:
        *_currentState = _tagsListState->EnterState(MODE_SELECT);
        break;
    case SELECT_COUNTRY:
        *_currentState = _countriesListState->EnterState(MODE_SELECT);
        break;
    case SELECT_SETTINGS:
        *_currentState = _selectSettingsState->EnterState(MODE_SELECT);
        break;
    case SLEEP:
        GoSleep();
        break;
    }

    return true;
}

void SelectModeState::GoSleep()
{
    _display->clearDisplay();
    delay(1000);
    esp_deep_sleep_start();
}

#endif