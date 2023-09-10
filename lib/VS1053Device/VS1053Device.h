#pragma once
#include <VS1053.h>
#define VS1053_CS 7
#define VS1053_DCS 15
#define VS1053_DREQ 16

#define VOLUME 80

class VS1053Device
{
private:
    VS1053 *player = nullptr;
    int currentVolume;

public:
    void Init();
    void Play(uint8_t *buffer, size_t length);
    void CloseDevice();
    void VolumeUp();
    void VolumeDown();
};

void VS1053Device::Init()
{
    delay(3000);
    currentVolume = VOLUME;
    SPI.begin(4, 5, 6);
    player = new VS1053(VS1053_CS, VS1053_DCS, VS1053_DREQ);
    player->begin();
    player->loadDefaultVs1053Patches();
    player->switchToMp3Mode();
    player->setVolume(currentVolume);
}

void VS1053Device::VolumeUp()
{
    if(currentVolume>=100)
        return;
    currentVolume+=10;
    player->setVolume(currentVolume);
}

void VS1053Device::VolumeDown()
{
    if(currentVolume<=0)
        return;
    currentVolume-=10;
    player->setVolume(currentVolume);
}

void VS1053Device::Play(uint8_t *buffer, size_t length)
{
    if (length == 32)
        player->playChunk(buffer, length);
    else
    {
        int toPlay = length;
        while (toPlay > 0)
        {
            if (toPlay >= 32)
            {
                player->playChunk(buffer, 32);
                buffer += 32;
                toPlay -= 32;
            }
            else
            {
                player->playChunk(buffer, toPlay);
                toPlay-=toPlay;
            }
        }
    }
}

void VS1053Device::CloseDevice()
{
    if (player)
        delete (player);
}