#ifndef TAGSLISTSTATE_H
#define TAGSLISTSTATE_H
#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <RadioListHttpClient.h>

class TagsListState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    StationsListState *_stationsListState;

public:
    TagsListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, StationsListState *stationsListState);
    UIState EnterState(UIState lastState);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetTagsPage();
    vector<TagDTO> tags;
    int currentIndex = 0;
    RadioListHttpClient radioListClient;

    bool HandleEnter();
    bool HandleBack();
};

TagsListState::TagsListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, StationsListState *stationsListState)
{
    _stationsListState = stationsListState;
    _currentState = currentState;
    _display = display;
}

void TagsListState::GetTagsPage()
{
    tags = radioListClient.GetTags();
}

UIState TagsListState::EnterState(UIState lastState)
{
    lastState = _lastState;
    tags = radioListClient.GetTags();
    return SELECT_TAG;
}

void TagsListState::HandleLoop()
{
    if (*_currentState != SELECT_TAG)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        for (int i = 0; i < tags.size(); i++)
        {
            char rowText[120];
            sprintf(rowText, "%s (%i)", tags[i].name.length() > 20 ? tags[i].name.substring(0, 20).c_str() : tags[i].name.c_str(), tags[i].count);
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

bool TagsListState::HandleBack()
{
    if (*_currentState != SELECT_TAG)
        return false;
    *_currentState =  _lastState;
    return true;
}

void TagsListState::HandleUp()
{
    if (*_currentState != SELECT_TAG)
        return;

    if (currentIndex > 0)
    {
        currentIndex--;
    }
    else
    {
        radioListClient.SetPrevTagsPage();
        GetTagsPage();
        currentIndex = tags.size() - 1;
    }
}

void TagsListState::HandleDown()
{
    if (*_currentState != SELECT_TAG)
        return;

    if (currentIndex < tags.size() - 1)
    {
        currentIndex++;
    }
    else
    {
        radioListClient.SetNextTagsPage();
        GetTagsPage();
        currentIndex = 0;
    }
}

bool TagsListState::HandleEnter()
{
    if (*_currentState != SELECT_TAG)
        return false;

    *_currentState = _stationsListState->EnterState(*_currentState, "", tags[currentIndex].name, TAG);
    return true;
}

void TagsListState::HandleRight()
{
    if (*_currentState != SELECT_TAG)
        return;

    radioListClient.SetNextTagsPage();
    GetTagsPage();
    currentIndex = 0;
}

void TagsListState::HandleLeft()
{
    if (*_currentState != SELECT_TAG)
        return;

    radioListClient.SetPrevTagsPage();
    GetTagsPage();
    currentIndex = 0;
}

#endif