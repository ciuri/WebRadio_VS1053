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

public:
    UIState EnterState(UIState lastState);
    void HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display);
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetTagsPage();
    vector<SearchModeDTO> modes;
    int currentIndex = 0;
 
    SearchModeDTO HandleEnter();
    UIState HandleBack();
};

UIState SelectModeState::EnterState(UIState lastState)
{
    _lastState = lastState;
    modes.clear();

    SearchModeDTO byCountry;
    byCountry.name = "Select by coutry";
    byCountry.selectBy = COUNTRY;

    SearchModeDTO byTag;
    byTag.name = "Select by tag";
    byTag.selectBy = TAG;
    

    modes.push_back(byCountry);
    modes.push_back(byTag);
    return MODE_SELECT;
}

void SelectModeState::HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display)
{
    display->setFont(u8g2_font_ncenB08_tr);
    display->firstPage();
    do
    {
        display->setCursor(0, 20);
        display->printf(modes[currentIndex].name.c_str());
    } while (display->nextPage());
}

UIState SelectModeState::HandleBack()
{
    return _lastState;
}

void SelectModeState::HandleUp()
{
    if (currentIndex > 0)
    {
        currentIndex--;
    }   
}

void SelectModeState::HandleDown()
{
    if (currentIndex < modes.size() - 1)
    {
        currentIndex++;
    }   
}

SearchModeDTO SelectModeState::HandleEnter()
{
    return modes[currentIndex];
}

#endif