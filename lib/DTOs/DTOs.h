#ifndef RADIOSTATIONDTO_H
#define RADIOSTATIONDTO_H

using namespace std;
class RadioStationDTO
{
public:
    String Name;
    String  Url;
    int  Bitrate;
    String  RadioFrequency;
    String Country;
};

class CountryDTO
{
    public:
    String code;
    String name;
    int count;
};

class TagDTO
{
    public:
    String name;
    int count;
};

enum UIState
{
    MODE_SELECT,
    SELECT_TAG,
    SELECT_COUNTRY,
    SELECT_STATION,
    PLAY,
    DEVICE_START,
    SELECT_SETTINGS,
    SELECT_SERVER_SETTING,
    SELECT_WIFI_SETTING    
};

class NamedModeDTO
{
    public:
    String name;
    UIState state;
};



enum DeviceStartStage
{
    WIFI,
    AUDIO
};

#endif