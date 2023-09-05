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
    PLAY
};

enum SelectBy
{
    TAG,
    COUNTRY,
    NAME
};

#endif