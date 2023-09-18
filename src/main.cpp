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

const char *ssid = "UPC97C7D2D";
const char *password = "jw4hejbQpcpk";

UIState currentState;
VS1053Device vs1053;

// U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0); //Dzialajacy

// nowy - ssd1309
U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI display(U8G2_R0, 13, 12, 14, 10, 11);

unsigned long lastDisplayUpdateMillis;

PlayingState playingState(&vs1053, &currentState, &display);
StationsListState stationsListState(&currentState, &display, &playingState);
CountriesListState countriesListState(&currentState, &display, &stationsListState);
TagsListState tagsListState(&currentState, &display, &stationsListState);
SelectModeState selectModeState;

static void HandleLoop(void *parameters);

void setup()
{
  esp_task_wdt_init(30, false);
  Serial.begin(115200);
  Serial.print("Wifi connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi connected.");
  vs1053.Init();
  StartAudioPlayTask();
  display.begin();
  currentState = selectModeState.EnterState(MODE_SELECT);
  xTaskCreate(HandleLoop, "HandleLoop", 16000, NULL, 3, NULL);
}

static void HandleLoop(void *parameters)
{
  while (true)
  {

    playingState.HandleLoop();
    stationsListState.HandleLoop();
    countriesListState.HandleLoop();
    tagsListState.HandleLoop();

    if (currentState == MODE_SELECT)
      selectModeState.HandleLoop(&display);

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
        // down

        if (currentState == MODE_SELECT)
        {
          selectModeState.HandleDown();
        }
      }
      if (incomingByte == 119)
      {
        stationsListState.HandleUp();
        countriesListState.HandleUp();
        tagsListState.HandleUp();
        // up

        playingState.HandleUp();
        if (currentState == MODE_SELECT)
        {
          selectModeState.HandleUp();
        }
      }
      if (incomingByte == 10)
      {

        switch (currentState)
        {

        case MODE_SELECT:
        {
          SearchModeDTO searchModeDto = selectModeState.HandleEnter();
          if (searchModeDto.selectBy == TAG)
            currentState = tagsListState.EnterState(currentState);
          else
            currentState = countriesListState.EnterState(currentState);
          break;
        }
        }

        stationsListState.HandleEnter() || countriesListState.HandleEnter() || tagsListState.HandleEnter();
      }
      if (incomingByte == 100)
      {
        stationsListState.HandleRight();
        tagsListState.HandleRight();
        // right
      }
      if (incomingByte == 97)
      {
        // left
        playingState.HandleBack() || stationsListState.HandleBack() || countriesListState.HandleBack() || tagsListState.HandleBack();
      }
    }
  }
}

void loop()
{
  delay(10);
}
