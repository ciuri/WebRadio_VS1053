#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H
#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include "VS1053Device.h"

class PlayingState
{
private:
    UIState _lastState;
    RadioStationDTO currentStation;
    VS1053Device *_vs1053;
    u8g2_uint_t offset; // current offset for the scrolling text
    u8g2_uint_t width;
    const uint8_t* GetFontForLength(int length);

public:
    PlayingState(VS1053Device *vs1053);
    UIState EnterState(UIState lastState, RadioStationDTO station);
    void HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display);
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    UIState HandleEnter();
    UIState HandleBack();
};

PlayingState::PlayingState(VS1053Device *vs1053)
{
    _vs1053 = vs1053;
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

void PlayingState::HandleLoop(U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display)
{ //u8g2_font_t0_22_tf


    
    display->firstPage();
    do
    {        
        display->setFont(GetFontForLength(currentStation.Name.length()));    
        display->setCursor(0, 20);
        display->printf(currentStation.Name.c_str());
        display->setFont(u8g2_font_tiny5_tf);
        display->setCursor(0,30);
        display->printf(currentStation.Country.c_str());
        display->setCursor(60, 60);
        display->printf("\nBuffer size: %i\n", circularBuffer.size());
        display->setCursor(0, 60);
        display->printf("Bitrate: %i", currentStation.Bitrate);
        if (chunked)
        {
            display->setCursor(0, 50);
            display->printf("Chunked");
        }
    } while (display->nextPage());
}

UIState PlayingState::HandleBack()
{
    stopPlaying = true;
    WaitForState(STOPPED);
    return _lastState;
}

void PlayingState::HandleUp()
{
    vs1053.VolumeUp();
}

void PlayingState::HandleDown()
{
    vs1053.VolumeDown();
}

UIState PlayingState::HandleEnter()
{
    return PLAY;
}
#endif