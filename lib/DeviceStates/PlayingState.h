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

public:
    PlayingState(VS1053Device *vs1053);
    UIState EnterState(UIState lastState, RadioStationDTO station);
    void HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display);
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

void PlayingState::HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display)
{
    display->setFont(u8g2_font_ncenB08_tr);
    display->firstPage();
    do
    {
        display->setCursor(0, 20);
        display->printf(currentStation.Name.c_str());
        display->setCursor(0, 40);
        display->printf("\nBuffer size: %i\n", circularBuffer.size());
        display->setCursor(0, 60);
        display->printf("Bitrate: %i", currentStation.Bitrate);
        if (chunked)
        {
            display->setCursor(70, 60);
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