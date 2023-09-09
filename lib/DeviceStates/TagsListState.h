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

public:
    UIState EnterState(UIState lastState);
    void HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display);
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetTagsPage();
    vector<TagDTO> tags;
    int currentIndex = 0;
    RadioListHttpClient radioListClient;

    TagDTO HandleEnter();
    UIState HandleBack();
};

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

void TagsListState::HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display)
{
    display->setFont(u8g2_font_ncenB08_tr);
    display->firstPage();
    do
    {
        display->setCursor(0, 20);
        display->printf(tags[currentIndex].name.c_str());
        display->setCursor(0, 40);
        display->printf("Stations: %i ", tags[currentIndex].count);
    } while (display->nextPage());
}

UIState TagsListState::HandleBack()
{
    return _lastState;
}

void TagsListState::HandleUp()
{
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

TagDTO TagsListState::HandleEnter()
{
    return tags[currentIndex];
}

void TagsListState::HandleRight()
{
    radioListClient.SetNextTagsPage();
    GetTagsPage();
    currentIndex = 0;
}

void TagsListState::HandleLeft()
{
    radioListClient.SetPrevTagsPage();
    GetTagsPage();
    currentIndex = 0;
}

#endif