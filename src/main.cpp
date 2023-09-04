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
#include <StationsListByCountryState.h>

const char *ssid = "UPC97C7D2D";
const char *password = "jw4hejbQpcpk";

UIState currentState;
VS1053Device vs1053;

// U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE, 8, 18);
U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0);

unsigned long lastDisplayUpdateMillis;
PlayingState playingState;
StationsListByCountryState stationsListByCountryState;

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
  currentState = stationsListByCountryState.EnterState(SELECT_COUNTRY);
}

void loop()
{
  if(currentState == PLAY)
    playingState.HandleLoop(&display);
  if(currentState == SELECT_BY_COUNTRY)
    stationsListByCountryState.HandleLoop(&display);

  if (Serial.available() > 0)
  {
    char incomingByte = Serial.read();
    // Serial.print("I received: ");
    // Serial.println(incomingByte, DEC);

    if (incomingByte == 51)
    {
      StopPlayTask();
    }

    if (incomingByte == 52)
    {
      ToggleChunking();
    }
    if (incomingByte == 115)
    {
      // down
      if (currentState == SELECT_BY_COUNTRY)
      {
        stationsListByCountryState.HandleDown();
      }
    }
    if (incomingByte == 119)
    {
      //up
       if (currentState == SELECT_BY_COUNTRY)
      {
        stationsListByCountryState.HandleUp();
      }
    }
    if (incomingByte == 10)
    {
      if (currentState == SELECT_BY_COUNTRY)
      {
        RadioStationDTO stationToPlay = stationsListByCountryState.HandleEnter();
        currentState = playingState.EnterState(currentState, stationToPlay);
      }
    }
    if (incomingByte == 100)
    {
      // right
      if (currentState == SELECT_BY_COUNTRY)
      {
        stationsListByCountryState.HandleRight();
      }
    }
    if (incomingByte == 97)
    {
      // left
       if (currentState == SELECT_BY_COUNTRY)
      {
        stationsListByCountryState.HandleLeft();
      }
       if (currentState == PLAY)
      {
         currentState = playingState.HandleBack();
      }
    }
  }
}
