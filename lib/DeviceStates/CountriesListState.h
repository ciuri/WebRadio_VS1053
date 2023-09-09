#ifndef STATIONSLISTBYCOUNTRYSTATE_H
#define STATIONSLISTBYCOUNTRYSTATE_H
#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <RadioListHttpClient.h>

class CountriesListState
{
private:
    UIState _lastState;

public:
    UIState EnterState(UIState lastState);
    void HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display);
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetCountriesPage();
    vector<CountryDTO> countries;
    int currentIndex = 0;
    RadioListHttpClient radioListClient;

    CountryDTO HandleEnter();
    UIState HandleBack();
};

void CountriesListState::GetCountriesPage()
{    
    countries = radioListClient.GetCountries();
}

UIState CountriesListState::EnterState(UIState lastState)
{
    lastState = _lastState;      
    countries = radioListClient.GetCountries();
    return SELECT_COUNTRY;
}

void CountriesListState::HandleLoop(U8G2_SH1106_128X64_NONAME_1_HW_I2C *display)
{
    display->setFont(u8g2_font_ncenB08_tr);
    display->firstPage();
    do
    {
        display->setCursor(0, 20);
        display->printf(countries[currentIndex].name.c_str());
        display->setCursor(0, 40);
        display->printf("Stations: %i ", countries[currentIndex].count);
    } while (display->nextPage());
}

UIState CountriesListState::HandleBack()
{
    return _lastState;
}

void CountriesListState::HandleUp()
{
    if (currentIndex > 0)
    {
        currentIndex--;
    }
    else
    {
        radioListClient.SetPrevCountriesPage();
        GetCountriesPage();
        currentIndex = countries.size() - 1;
    }
}

void CountriesListState::HandleDown()
{
    if (currentIndex < countries.size() - 1)
    {
        currentIndex++;
    }
    else
    {
        radioListClient.SetNextCountriesPage();
        GetCountriesPage();
        currentIndex = 0;
    }
}

CountryDTO CountriesListState::HandleEnter()
{
    return countries[currentIndex];
}

void CountriesListState::HandleRight()
{
    radioListClient.SetNextStationsPage();
    GetCountriesPage();
    currentIndex = 0;
}

void CountriesListState::HandleLeft()
{
    radioListClient.SetPrevStationsPage();
    GetCountriesPage();
    currentIndex = 0;
}

#endif