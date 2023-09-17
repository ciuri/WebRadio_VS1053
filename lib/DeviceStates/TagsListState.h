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
    void HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display);
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

void TagsListState::HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display)
{
    display->setFont(u8g2_font_NokiaSmallPlain_tf);
    display->firstPage();
    do
    {
         for (int i = 0; i < tags.size(); i++)
        {
            char rowText[120];
            sprintf(rowText, "%s (%i)",tags[i].name.length()>20?tags[i].name.substring(0,20).c_str():tags[i].name.c_str(), tags[i].count);
            display->setCursor(0, i * 10 + 8);
            if (i != currentIndex)
            {
             display->printf(rowText);   
            }
            else
            {                
                display->drawButtonUTF8(2, i*10+8, U8G2_BTN_BW1, 120, 1, 0, rowText);
            }
        }

        /*display->setCursor(0, 10);
        display->printf(tags[currentIndex].name.c_str());
        display->setCursor(0, 20);
        display->printf("Stations: %i ", tags[currentIndex].count);*/
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