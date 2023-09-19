#ifndef DEVICESTARTSTATE_H
#define DEVICESTARTSTATE_H

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>

class DeviceStartState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;    
    SelectModeState *_selectModeState;
    DeviceStartStage _startStage = WIFI;
    const char *ssid = "UPC97C7D2D";
const char *password = "jw4hejbQpcpk";

public:
    DeviceStartState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display,   SelectModeState *selectModeState);
    UIState EnterState();
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

DeviceStartState::DeviceStartState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display,  SelectModeState *selectModeState)
{
    _currentState = currentState;
    _display = display;  
    _selectModeState = selectModeState;
}

UIState DeviceStartState::EnterState()
{
    *_currentState = DEVICE_START;  
     Serial.begin(115200);
    Serial.print("Wifi connecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Wifi connected.");
    _startStage = AUDIO;
    vs1053.Init();
    StartAudioPlayTask();
    return  _selectModeState->EnterState(MODE_SELECT);    
}

void DeviceStartState::HandleLoop()
{
    if (*_currentState != DEVICE_START)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        _display->setCursor(0,10);
        _display->printf("WIFI connecting...");
        if(_startStage == AUDIO)
        {
            _display->setCursor(0,20);
            _display->printf("Audio initialization...");               
        }
        
    } while (_display->nextPage());
}

#endif