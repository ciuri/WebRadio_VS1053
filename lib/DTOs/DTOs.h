#ifndef RADIOSTATIONDTO_H
#define RADIOSTATIONDTO_H
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1

#include <ArduinoJson.h>

using namespace std;
class RadioStationDTO
{
public:
    String Name;
    String Url;
    int Bitrate;
    String RadioFrequency;
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
    SELECT_FAVORITES,
    PLAY,
    DEVICE_START,
    SELECT_SETTINGS,
    SELECT_SERVER_SETTING,
    SELECT_WIFI_SETTING,
    ENTER_PASSWORD,
    RESTART
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