#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1

#include <WiFi.h>
#include <HTTPClient.h>
#include "VS1053Device.h"
#include "MusicStream.h"
#include <HttpWebRadioClient.h>
#include <RadioListHttpClient.h>
#include <esp_task_wdt.h>
#include <DTOs.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <PlayingState.h>
#include <StationsListState.h>
#include <CountriesListState.h>
#include <TagsListState.h>
#include <SelectModeState.h>
#include <DeviceStartState.h>
#include <AiEsp32RotaryEncoder.h>
#include <SelectSettingsState.h>
#include <SelectServerState.h>
#include <WiFiListState.h>
#include <FS.h>
#include <SPIFFS.h>
#include <DeviceConfiguration.h>

#define FORMAT_SPIFFS_IF_FAILED true
#define ROTARY_ENCODER_A_PIN 42
#define ROTARY_ENCODER_B_PIN 2
#define ROTARY_ENCODER_BUTTON_PIN 1
#define ROTARY_ENCODER_STEPS 4

DeviceConfiguration config;
UIState currentState;
VS1053Device vs1053;
DeviceStartStage _startStage = WIFI;
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
RadioListHttpClient radioListClient(&config);

// U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0); //Dzialajacy

// nowy - ssd1309
U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI display(U8G2_R0, 13, 12, 14, 10, 11);

unsigned long lastDisplayUpdateMillis;

PlayingState playingState(&vs1053, &currentState, &display, &config);
SelectFavoritesState selectFavoritesState(&currentState, &display, &playingState, &config);
StationsListState stationsListState(&currentState, &display, &playingState, &radioListClient);
CountriesListState countriesListState(&currentState, &display, &stationsListState, &radioListClient);
TagsListState tagsListState(&currentState, &display, &stationsListState, &radioListClient);
SelectServerState selectServerState(&currentState, &display, &config);
WiFiListState wifiListState(&currentState, &display);
SelectSettingsState selectSettingsState(&currentState, &display, &countriesListState, &tagsListState, &selectServerState, &wifiListState);
SelectModeState selectModeState(&currentState, &display, &countriesListState, &tagsListState, &selectSettingsState, &selectFavoritesState);
DeviceStartState deviceStartState(&currentState, &display, &selectModeState, &config);

void HandleUp();
void HandleDown();
void HandleLeft();
void HandleRight();
void HandleEnter();
void HandleBack();
void HandleLoops();

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}

static void HandleLoop(void *parameters);

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  config.LoadConfiguration();
  //config.favorites.clear();
  //config.SaveConfiguration();
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(0, 1000, true);
  rotaryEncoder.setAcceleration(250);

  display.begin();
  xTaskCreate(HandleLoop, "HandleLoop", 16000, NULL, 3, NULL);
  currentState = deviceStartState.EnterState();
  esp_task_wdt_init(30, false);
}

void HandleLoops()
{
  playingState.HandleLoop();
  stationsListState.HandleLoop();
  countriesListState.HandleLoop();
  tagsListState.HandleLoop();
  selectModeState.HandleLoop();
  deviceStartState.HandleLoop();
  selectSettingsState.HandleLoop();
  selectServerState.HandleLoop();
  wifiListState.HandleLoop();
  selectFavoritesState.HandleLoop();
}

void HandleDown()
{
  stationsListState.HandleDown();
  countriesListState.HandleDown();
  tagsListState.HandleDown();
  playingState.HandleDown();
  selectModeState.HandleDown();
  selectSettingsState.HandleDown();
  selectServerState.HandleDown();
  wifiListState.HandleDown();
  selectFavoritesState.HandleDown();
}

void HandleUp()
{
  stationsListState.HandleUp();
  countriesListState.HandleUp();
  tagsListState.HandleUp();
  selectModeState.HandleUp();
  playingState.HandleUp();
  selectSettingsState.HandleUp();
  selectServerState.HandleUp();
  wifiListState.HandleUp();
  selectFavoritesState.HandleUp();
}

void HandleEnter()
{
  stationsListState.HandleEnter() || countriesListState.HandleEnter() || tagsListState.HandleEnter() || selectModeState.HandleEnter() || selectSettingsState.HandleEnter() || selectServerState.HandleEnter() || wifiListState.HandleEnter() || selectFavoritesState.HandleEnter() || playingState.HandleEnter();
}

void HandleRight()
{
  stationsListState.HandleRight();
  tagsListState.HandleRight();
}

void HandleBack()
{
  playingState.HandleBack() || stationsListState.HandleBack() || countriesListState.HandleBack() || tagsListState.HandleBack() || selectSettingsState.HandleBack() || selectServerState.HandleBack() || wifiListState.HandleBack() || selectFavoritesState.HandleBack();
  ;
}

unsigned long shortPressAfterMiliseconds = 50;
unsigned long longPressAfterMiliseconds = 350; 

void on_button_short_click()
{
  HandleEnter();
}

void on_button_long_click()
{
  HandleBack();
}

void handle_rotary_button()
{
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();  

  if (isEncoderButtonDown)
  {   
    if (!wasButtonDown)
    {
      lastTimeButtonDown = millis();
    }
    wasButtonDown = true;
    return;
  }
  
  if (wasButtonDown)
  { 
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds)
    {
      on_button_long_click();
    }
    else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds)
    {
      on_button_short_click();
    }
  }
  wasButtonDown = false;
}

static void HandleLoop(void *parameters)
{
  while (true)
  {
    delay(10);
    HandleLoops();

    if (Serial.available() > 0)
    {
      char incomingByte = Serial.read();
      Serial.print("I received: ");
      Serial.println(incomingByte, DEC);

      if (incomingByte == 115)
      {
        HandleDown();
      }
      if (incomingByte == 119)
      {
        HandleUp();
      }

      if (incomingByte == 10)
      {
        HandleEnter();
      }

      if (incomingByte == 100)
      {
        HandleRight();
      }

      if (incomingByte == 97)
      {
        HandleBack();
      }
    }

    int encoderChanged = rotaryEncoder.encoderChanged();
    if (encoderChanged)
    {
      if (encoderChanged < 0)
        HandleDown();
      if (encoderChanged > 0)
        HandleUp();
    }
    handle_rotary_button();
  }
}

void loop()
{
  delay(10);
}
