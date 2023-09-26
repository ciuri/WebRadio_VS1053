#ifndef SELECTSETTINGSSTATE_H
#define SELECTSETTINGSSTATE_H

#define LIST_START_OFFSET 20

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <SelectServerState.h>

class SelectSettingsState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    CountriesListState *_countriesListState;
    TagsListState *_tagsListState;
    SelectServerState *_selectServerState;

public:
    SelectSettingsState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, CountriesListState *countriesListState, TagsListState *tagsListState, SelectServerState *selectServerState);
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
};

SelectSettingsState::SelectSettingsState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, CountriesListState *countriesListState, TagsListState *tagsListState, SelectServerState *selectServerState)
{
    _currentState = currentState;
    _display = display;
    _countriesListState = countriesListState;
    _tagsListState = tagsListState;
    _selectServerState = selectServerState;
}

UIState SelectSettingsState::EnterState(UIState lastState)
{
    _lastState = lastState;
    modes.clear();

    NamedModeDTO servers;
    servers.name = "Select radio list server";
    servers.state = SELECT_SERVER_SETTING;

    NamedModeDTO wifi;
    wifi.name = "WiFi settings";
    wifi.state = SELECT_TAG;

    modes.push_back(servers);
    modes.push_back(wifi);
    return SELECT_SETTINGS;
}

void SelectSettingsState::HandleLoop()
{
    if (*_currentState != SELECT_SETTINGS)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        _display->setCursor(40, 8);
        _display->printf("SETTINGS");

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

bool SelectSettingsState::HandleBack()
{
    if (*_currentState != SELECT_SETTINGS)
        return false;

    *_currentState = _lastState;
    return true;
}

void SelectSettingsState::HandleUp()
{
    if (*_currentState != SELECT_SETTINGS)
        return;

    if (currentIndex > 0)
    {
        currentIndex--;
    }
}

void SelectSettingsState::HandleDown()
{
    if (*_currentState != SELECT_SETTINGS)
        return;

    if (currentIndex < modes.size() - 1)
    {
        currentIndex++;
    }
}

bool SelectSettingsState::HandleEnter()
{
    if (*_currentState != SELECT_SETTINGS)
        return false;

    switch (modes[currentIndex].state)
    {
    case SELECT_SERVER_SETTING:
        *_currentState = _selectServerState->EnterState(SELECT_SETTINGS);
        break;
    }

    return true;
}

#endif