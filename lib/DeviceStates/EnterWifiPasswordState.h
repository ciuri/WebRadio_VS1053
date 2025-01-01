#ifndef ENTER_PASSWORD_STATE
#define ENTER_PASSWORD_STATE

#define LIST_START_OFFSET 20

#include <DeviceStateBase.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <DTOs.h>
#include <DeviceConfiguration.h>


class EnterWifiPasswordState : public DeviceStateBase
{
private:
    UIState _lastState;
    UIState *_currentState;
    U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *_display;
    DeviceConfiguration *_config;
    char _currentChar;
    int _currentCharIndex = 32;
    char _password[64];
    char _tmpPassword[64];
    String _wifiName;

public:
    EnterWifiPasswordState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, DeviceConfiguration *config);
    UIState EnterState(UIState lastState, String wifiName);
    void HandleLoop();
    void HandleUp();
    void HandleDown();
    void HandleLeft() {};
    void HandleRight() {};
    void GetTagsPage();

    bool HandleEnter();
    bool HandleBack();
};

EnterWifiPasswordState::EnterWifiPasswordState(UIState *currentState, U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI *display, DeviceConfiguration *config)
{
    _currentState = currentState;
    _display = display;
    _config = config;
}

UIState EnterWifiPasswordState::EnterState(UIState lastState, String wifiName)
{
    _lastState = lastState;
    _wifiName = wifiName;
    return ENTER_PASSWORD;
}

void EnterWifiPasswordState::HandleLoop()
{
    if (*_currentState != ENTER_PASSWORD)
        return;

    _display->setFont(u8g2_font_NokiaSmallPlain_tf);
    _display->firstPage();
    do
    {
        _display->setCursor(0, 8);
        _display->printf("%s password:", _wifiName.c_str());
        _display->setCursor(0, 10 + LIST_START_OFFSET);
        char text[1];
        sprintf(text, "%c", _currentCharIndex);
        _display->println(text);
        _display->setCursor(0, 20 + LIST_START_OFFSET);
        _display->println(_password);

    } while (_display->nextPage());
}

bool EnterWifiPasswordState::HandleBack()
{
    if (*_currentState != ENTER_PASSWORD)
        return false;
    _config->wifiName = _wifiName;
    _config->wifiPassword = _password;
    _config->SaveConfiguration();
    ESP.restart();
    *_currentState = _lastState;
    return true;
}

void EnterWifiPasswordState::HandleUp()
{
    if (*_currentState != ENTER_PASSWORD)
        return;
    if (_currentCharIndex > 32)
    {
        _currentCharIndex--;
        Serial.printf("CharIndex: %i", _currentCharIndex);
    }
}

void EnterWifiPasswordState::HandleDown()
{
    if (*_currentState != ENTER_PASSWORD)
        return;

    _currentCharIndex++;
    Serial.printf("CharIndex: %i", _currentCharIndex);
}

bool EnterWifiPasswordState::HandleEnter()
{
    if (*_currentState != ENTER_PASSWORD)
        return false;
    sprintf(_tmpPassword, "%s%c", _password, _currentCharIndex);
    sprintf(_password, "%s", _tmpPassword);
    return true;
}

#endif