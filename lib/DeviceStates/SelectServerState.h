#ifndef SELECTSERVERSSTATE_H
#define SELECTSERVERSSTATE_H

#define LIST_START_OFFSET 20

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>



class SelectServerState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
   
public:
    SelectServerState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display);
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

SelectServerState::SelectServerState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display)
{
    _currentState = currentState;
    _display = display;
}

UIState SelectServerState::EnterState(UIState lastState)
{
    _lastState = lastState;
    modes.clear();

    NamedModeDTO nl1;
    nl1.name = "nl1.api.radio-browser.info";
    nl1.state = SELECT_SERVER_SETTING; 

    NamedModeDTO de1;
    de1.name = "de1.api.radio-browser.info";
    de1.state = SELECT_SERVER_SETTING;

    NamedModeDTO de2;
    de2.name = "de2.api.radio-browser.info";
    de2.state = SELECT_SERVER_SETTING;

    NamedModeDTO at1;
    at1.name = "at1.api.radio-browser.info";
    at1.state = SELECT_SERVER_SETTING;   

    modes.push_back(nl1);    
    modes.push_back(de1);
    modes.push_back(de2);
    modes.push_back(at1); 

    return SELECT_SERVER_SETTING;
}

void SelectServerState::HandleLoop()
{
    if (*_currentState != SELECT_SERVER_SETTING)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    { 
         _display->setCursor(35,8);
        _display->printf("SELECT SERVER");
    
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

bool SelectServerState::HandleBack()
{
    if (*_currentState != SELECT_SERVER_SETTING)
        return false;

    *_currentState = _lastState;
    return true;
}

void SelectServerState::HandleUp()
{
    if (*_currentState != SELECT_SERVER_SETTING)
        return;

    if (currentIndex > 0)
    {
        currentIndex--;
    }
}

void SelectServerState::HandleDown()
{
    if (*_currentState != SELECT_SERVER_SETTING)
        return;

    if (currentIndex < modes.size() - 1)
    {
        currentIndex++;
    }
}

bool SelectServerState::HandleEnter()
{
    if (*_currentState != SELECT_SERVER_SETTING)
        return false;

    

    return true;
}

#endif