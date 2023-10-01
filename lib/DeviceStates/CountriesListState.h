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
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    StationsListState *_stationsListState;
    RadioListHttpClient *_radioListClient;

public:
    CountriesListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, StationsListState *stationsListState, RadioListHttpClient *radioListClient);
    UIState EnterState(UIState lastState);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void GetCountriesPage();
    vector<CountryDTO> countries;
    int currentIndex = 0;

    bool HandleEnter();
    bool HandleBack();
};

CountriesListState::CountriesListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, StationsListState *stationsListState, RadioListHttpClient *radioListClient)
{
    _stationsListState = stationsListState;
    _currentState = currentState;
    _display = display;
    _radioListClient = radioListClient;
}

void CountriesListState::GetCountriesPage()
{
    countries = _radioListClient->GetCountries();
}

UIState CountriesListState::EnterState(UIState lastState)
{
    lastState = _lastState;
    countries = _radioListClient->GetCountries();
    return SELECT_COUNTRY;
}

void CountriesListState::HandleLoop()
{
    if (*_currentState != SELECT_COUNTRY)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        for (int i = 0; i < countries.size(); i++)
        {
            char rowText[120];
            sprintf(rowText, "%s (%i)", countries[i].name.length() > 20 ? countries[i].name.substring(0, 20).c_str() : countries[i].name.c_str(), countries[i].count);
            _display->setCursor(0, i * 10 + 8);
            if (i != currentIndex)
            {
                _display->printf(rowText);
            }
            else
            {
                _display->drawButtonUTF8(2, i * 10 + 8, U8G2_BTN_BW1, 120, 1, 0, rowText);
            }
        }
       
    } while (_display->nextPage());
}

bool CountriesListState::HandleBack()
{
    if (*_currentState != SELECT_COUNTRY)
        return false;

    *_currentState = _lastState;
    return true;
}

void CountriesListState::HandleUp()
{
    if (*_currentState != SELECT_COUNTRY)
        return;

    if (currentIndex > 0)
    {
        currentIndex--;
    }
    else
    {
        _radioListClient->SetPrevCountriesPage();
        GetCountriesPage();
        currentIndex = countries.size() - 1;
    }
}

void CountriesListState::HandleDown()
{
    if (*_currentState != SELECT_COUNTRY)
        return;

    if (currentIndex < countries.size() - 1)
    {
        currentIndex++;
    }
    else
    {
       _radioListClient->SetNextCountriesPage();
        GetCountriesPage();
        currentIndex = 0;
    }
}

bool CountriesListState::HandleEnter()
{
    if (*_currentState != SELECT_COUNTRY)
        return false;

    *_currentState = _stationsListState->EnterState(SELECT_COUNTRY, countries[currentIndex].code, "");
    return true;
}

void CountriesListState::HandleRight()
{
    if (*_currentState != SELECT_COUNTRY)
        return;

    _radioListClient->SetNextStationsPage();
    GetCountriesPage();
    currentIndex = 0;
}

void CountriesListState::HandleLeft()
{
    if (*_currentState != SELECT_COUNTRY)
        return;

    _radioListClient->SetPrevStationsPage();
    GetCountriesPage();
    currentIndex = 0;
}

#endif