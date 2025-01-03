#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H

#include <DeviceStateBase.h>
#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include "VS1053Device.h"
#include <DeviceConfiguration.h>


class PlayingState : public DeviceStateBase
{
private:
    UIState _lastState;
    UIState*  _currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI* _display;
    DeviceConfiguration* _config;
    RadioStationDTO currentStation;
    VS1053Device *_vs1053;
    u8g2_uint_t offset; // current offset for the scrolling text
    u8g2_uint_t width;
    const uint8_t* GetFontForLength(int length);
    unsigned long saveToFavTime;

public:
    PlayingState(VS1053Device *vs1053, UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, DeviceConfiguration* config);
    UIState EnterState(UIState lastState, RadioStationDTO station);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft() {};
    void HandleRight() {};
    bool HandleEnter();
    bool HandleBack();
    void AddToFavorites();
};

PlayingState::PlayingState(VS1053Device *vs1053, UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, DeviceConfiguration* config)
{
    _vs1053 = vs1053;
    _currentState = currentState;
    _display = display;
    _config = config;
}

UIState PlayingState::EnterState(UIState lastState, RadioStationDTO station)
{
    currentStation = station;
    _lastState = lastState;
    StopPlayTask();
    WaitForState(STOPPED);
    StartPlayTask(station.Url.c_str());
    return PLAY;
}

const uint8_t* PlayingState::GetFontForLength(int length)
{
    if(length<16)
        return u8g2_font_doomalpha04_tr;
    else if(length >= 16 && length <25)
        return u8g2_font_NokiaSmallBold_tf;
    else if(length >=25)
        return u8g2_font_tiny5_tf;
    
    return u8g2_font_tiny5_tf;
}

void PlayingState::HandleLoop()
{ 
    if(*_currentState!=PLAY)    
        return;

    _display->firstPage();
    do
    {        
        _display->setFont(GetFontForLength(currentStation.Name.length()));    
        _display->setCursor(0, 20);
        _display->printf(currentStation.Name.c_str());
        _display->setFont(u8g2_font_tiny5_tf);
        _display->setCursor(0,30);
        _display->printf(currentStation.Country.c_str());
        if(millis()-saveToFavTime<2000)
        {
            _display->setCursor(0, 40);
            _display->printf("Added to favories");
        }
        _display->setCursor(60, 60);
        _display->printf("\nBuffer size: %i\n", circularBuffer.size());
        _display->setCursor(0, 50);
        _display->printf("Volume: %i", vs1053.GetVolume());
        _display->setCursor(0, 60);
        _display->printf("Bitrate: %i", currentStation.Bitrate);
        if (chunked)
        {
            _display->setCursor(60, 50);
            _display->printf("Chunked");
        }
    } while (_display->nextPage());
}

bool PlayingState::HandleBack()
{ 
    if(*_currentState!=PLAY)    
        return false;

    stopPlaying = true;
    WaitForState(STOPPED);
    *_currentState = _lastState;
    return true;
}

void PlayingState::HandleUp()
{
     if(*_currentState!=PLAY)    
        return;
    vs1053.VolumeDown();
}

void PlayingState::HandleDown()
{
     if(*_currentState!=PLAY)    
        return;
    vs1053.VolumeUp();
}

bool PlayingState::HandleEnter()
{
     if(*_currentState!=PLAY)    
        return false;
  
    *_currentState = PLAY;
    return true;
}

void PlayingState::AddToFavorites()
{ 
    if(*_currentState!=PLAY)    
        return;  
    saveToFavTime = millis();
    _config->favorites.push_back(currentStation);
    _config->SaveConfiguration();

    *_currentState = PLAY;
}
#endif