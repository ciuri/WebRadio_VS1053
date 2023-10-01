#ifndef DEVICESTARTSTATE_H
#define DEVICESTARTSTATE_H

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <DeviceConfiguration.h>

class DeviceStartState
{
private:
    DeviceConfiguration* _config;
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    SelectModeState *_selectModeState;
    DeviceStartStage _startStage = WIFI;

public:
    DeviceStartState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, SelectModeState *selectModeState, DeviceConfiguration* config);
    UIState EnterState();
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

DeviceStartState::DeviceStartState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, SelectModeState *selectModeState, DeviceConfiguration* config)
{
    _currentState = currentState;
    _display = display;
    _selectModeState = selectModeState;
    _config = config;
}

UIState DeviceStartState::EnterState()
{
    *_currentState = DEVICE_START;  
    Serial.print("Wifi connecting...");   
    WiFi.begin(_config->wifiName.c_str(), _config->wifiPassword.c_str());
    unsigned long wifiConnectStartTime = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if(millis()-wifiConnectStartTime>10000)
        {
            Serial.println("Could not connect to WiFi");
            break;
        }
    }
    if(WiFi.status() == WL_CONNECTED)
        Serial.println("Wifi connected.");
    _startStage = AUDIO;
    vs1053.Init();
    StartAudioPlayTask();
    return _selectModeState->EnterState(MODE_SELECT);
}

void DeviceStartState::HandleLoop()
{
    if (*_currentState != DEVICE_START)
        return;
   
    _display->firstPage();
    do
    {
        _display->setFont(u8g2_font_spleen12x24_mf);
        _display->setCursor(0, 20);
        _display->printf("WebRadio");
        _display->setFont(u8g2_font_trixel_square_tf );
        _display->setCursor(0, 50);
        _display->printf("WIFI connecting...");
        if (_startStage == AUDIO)
        {
            _display->setCursor(0, 60);
            _display->printf("Audio initialization...");
        }

    } while (_display->nextPage());
}

#endif