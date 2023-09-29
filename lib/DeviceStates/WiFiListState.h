#ifndef WIFILISTSTATE_H
#define WIFILISTSTATE_H

#define LIST_START_OFFSET 20

#include <HttpWebRadioClient.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>

class WiFiListState
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;

public:
    WiFiListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display);
    UIState EnterState(UIState lastState);
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

WiFiListState::WiFiListState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display)
{
    _currentState = currentState;
    _display = display;
}

UIState WiFiListState::EnterState(UIState lastState)
{
    _lastState = lastState;
    modes.clear();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    int networkCount = WiFi.scanNetworks();

    for (int i = 0; i < networkCount; i++)
    {
        NamedModeDTO n;
        n.name = WiFi.SSID(i);
        n.state = SELECT_SERVER_SETTING;
        modes.push_back(n);
        Serial.println(WiFi.SSID(i));
    }
    return SELECT_WIFI_SETTING;
}

void WiFiListState::HandleLoop()
{
    if (*_currentState != SELECT_WIFI_SETTING)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        _display->setCursor(35, 8);
        _display->printf("SELECT WiFi");

        int itemsPerPage = 5;
        int dataCount = modes.size();
        int page = currentIndex / itemsPerPage;

        for (int i = itemsPerPage * page; i < itemsPerPage * page + itemsPerPage && i < dataCount; i++)
        {
            char rowText[120];
            sprintf(rowText, "%s", modes[i].name.c_str());
            _display->setCursor(0, i % itemsPerPage * 10 + LIST_START_OFFSET);
            if (i != currentIndex)
            {
                _display->printf(rowText);
            }
            else
            {
                _display->drawButtonUTF8(2, i % itemsPerPage * 10 + LIST_START_OFFSET, U8G2_BTN_BW1, 120, 1, 0, rowText);
            }
        }
    } while (_display->nextPage());
}

bool WiFiListState::HandleBack()
{
    if (*_currentState != SELECT_WIFI_SETTING)
        return false;

    *_currentState = _lastState;
    return true;
}

void WiFiListState::HandleUp()
{
    if (*_currentState != SELECT_WIFI_SETTING)
        return;

    if (currentIndex > 0)
    {
        currentIndex--;
    }
}

void WiFiListState::HandleDown()
{
    if (*_currentState != SELECT_WIFI_SETTING)
        return;

    if (currentIndex < modes.size() - 1)
    {
        currentIndex++;
    }
}

bool WiFiListState::HandleEnter()
{
    if (*_currentState != SELECT_WIFI_SETTING)
        return false;

    return true;
}

#endif