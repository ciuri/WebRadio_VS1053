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
    void HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display);
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
    byCountry.name = "Select by country";
    byCountry.selectBy = COUNTRY;

    SearchModeDTO byTag;
    byTag.name = "Select by tag";
    byTag.selectBy = TAG;
    

    modes.push_back(byCountry);
    modes.push_back(byTag);
    return MODE_SELECT;
}

void SelectModeState::HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display)
{
    display->setFont(u8g2_font_NokiaSmallPlain_tf);
    display->firstPage();
    do
    {
         for (int i = 0; i < modes.size(); i++)
        {
            char rowText[120];
            sprintf(rowText, "%s", modes[i].name.c_str());
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
        display->printf(modes[currentIndex].name.c_str());*/
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