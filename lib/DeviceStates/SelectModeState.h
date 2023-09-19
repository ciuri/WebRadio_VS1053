#ifndef SELECTMODESTATE_H
#define SELECTMODESTATE_H

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>

class SelectModeState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    CountriesListState *_countriesListState;
    TagsListState *_tagsListState;

public:
    SelectModeState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, CountriesListState *countriesListState, TagsListState *tagsListState);
    UIState EnterState(UIState lastState);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetTagsPage();
    vector<SearchModeDTO> modes;
    int currentIndex = 0;

    bool HandleEnter();
    bool HandleBack();
};

SelectModeState::SelectModeState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, CountriesListState *countriesListState, TagsListState *tagsListState)
{
    _currentState = currentState;
    _display = display;
    _countriesListState = countriesListState;
    _tagsListState = tagsListState;
}

UIState SelectModeState::EnterState(UIState lastState)
{
    _lastState = lastState;
    modes.clear();

    SearchModeDTO byCountry;
    byCountry.name = "Select by country";
    byCountry.selectBy = COUNTRY;

    SearchModeDTO byTag;
    byTag.name = "Select by tag";
    byTag.selectBy = TAG;

    modes.push_back(byCountry);
    modes.push_back(byTag);
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
        for (int i = 0; i < modes.size(); i++)
        {
            char rowText[120];
            sprintf(rowText, "%s", modes[i].name.c_str());
            _display->setCursor(0, i * 10 + 8);
            if (i != currentIndex)
            {
                _display->printf(rowText);
            }
            else
            {
                _display->drawButtonUTF8(2, i * 10 + 8, U8G2_BTN_BW1, 120, 1, 0, rowText);
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

    if (modes[currentIndex].selectBy == TAG)
        *_currentState = _tagsListState->EnterState(MODE_SELECT);
    else
        *_currentState = _countriesListState->EnterState(MODE_SELECT);

    return true;
}

#endif