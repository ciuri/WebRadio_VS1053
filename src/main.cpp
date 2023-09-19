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

UIState currentState;
VS1053Device vs1053;
DeviceStartStage _startStage = WIFI;



// U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0); //Dzialajacy

// nowy - ssd1309
U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI display(U8G2_R0, 13, 12, 14, 10, 11);

unsigned long lastDisplayUpdateMillis;

PlayingState playingState(&vs1053, &currentState, &display);
StationsListState stationsListState(&currentState, &display, &playingState);
CountriesListState countriesListState(&currentState, &display, &stationsListState);
TagsListState tagsListState(&currentState, &display, &stationsListState);
SelectModeState selectModeState(&currentState, &display, &countriesListState, &tagsListState);
DeviceStartState deviceStartState(&currentState, &display, &selectModeState);

static void HandleLoop(void *parameters);

void setup()
{
  display.begin();
  xTaskCreate(HandleLoop, "HandleLoop", 16000, NULL, 3, NULL);
  currentState = deviceStartState.EnterState();
  esp_task_wdt_init(30, false); 
}

static void HandleLoop(void *parameters)
{
  while (true)
  {
    delay(10);
    playingState.HandleLoop();
    stationsListState.HandleLoop();
    countriesListState.HandleLoop();
    tagsListState.HandleLoop();
    selectModeState.HandleLoop();
    deviceStartState.HandleLoop();

    if (Serial.available() > 0)
    {
      char incomingByte = Serial.read();
      Serial.print("I received: ");
      Serial.println(incomingByte, DEC);

      if (incomingByte == 115)
      {
        stationsListState.HandleDown();
        countriesListState.HandleDown();
        tagsListState.HandleDown();
        playingState.HandleDown();
        selectModeState.HandleDown();
      }
      if (incomingByte == 119)
      {
        stationsListState.HandleUp();
        countriesListState.HandleUp();
        tagsListState.HandleUp();
        selectModeState.HandleUp();
        playingState.HandleUp();
      }

      if (incomingByte == 10)
      {
        stationsListState.HandleEnter() || countriesListState.HandleEnter() || tagsListState.HandleEnter() || selectModeState.HandleEnter();
      }

      if (incomingByte == 100)
      {
        stationsListState.HandleRight();
        tagsListState.HandleRight();
      }

      if (incomingByte == 97)
      {
        playingState.HandleBack() || stationsListState.HandleBack() || countriesListState.HandleBack() || tagsListState.HandleBack();
      }
    }
  }
}

void loop()
{
  delay(10);
}
