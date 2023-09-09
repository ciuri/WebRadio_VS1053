#ifndef HTTPWEBRADIOCLIENT_H
#define HTTPWEBRADIOCLIENT_H

#include <HTTPClient.h>
#include <esp_task_wdt.h>
#include <MusicStream.h>
#include <VS1053Device.h>
#include <CircularBuffer.h>
#include <mutex>

enum PlayerState
{
  STOPPED,
  PLAYING,
  WAITING,
  ERROR
};

uint8_t *mp3buff;
bool stopPlaying;
extern VS1053Device vs1053;
TaskHandle_t playTaskHandle = NULL;
TaskHandle_t playAudioTaskHandle = NULL;
String _currentURL = "";
PlayerState _playerState = STOPPED;
CircularBuffer<uint8_t, 16384> circularBuffer;
std::mutex mtx;
bool overwriteChunking = false;

static void PlayTask(void *parameters)
{
  esp_task_wdt_init(30, false);

  overwriteChunking = false;
  _playerState = WAITING;

  const char *CONTENT_TYPE = "Content-Type";
  const char *ICY_NAME = "icy-name";
  const char *ICY_METAINT = "icy-metaint";
  const char *ENCODING = "Transfer-Encoding";
  const char *BITRATE = "icy-br";
  const char *LOCATION = "Location";

  const char *header[] = {CONTENT_TYPE, ICY_NAME, ICY_METAINT,
                          ENCODING, BITRATE, LOCATION};

  HTTPClient webRadioHttpClient;
  MusicStream *_currentStream = NULL;
  bool chunked = false;
  Serial.println("Start Play Task");
  Serial.println(_currentURL);
  stopPlaying = false;
  // bool result = WebRadioHttpClient.begin("http://icy.unitedradio.it/RMC.mp3");
  // bool result = WebRadioHttpClient.begin("https://stream.open.fm/368?type=.aac&user=800043471858&player_group=WWW");
  bool result = webRadioHttpClient.begin(_currentURL);
  webRadioHttpClient.collectHeaders(header, sizeof(header) / sizeof(char *));

  int httpResult = webRadioHttpClient.GET();

  if (httpResult >= 300)
  {
    webRadioHttpClient.end();
    Serial.println("Ending play task");
    _playerState = STOPPED;
    vTaskDelete(playTaskHandle);
    return;
  }

  chunked = webRadioHttpClient.header("Transfer-Encoding").equals("chunked") ? true : false;
 
  while (true && !stopPlaying)
  {
    int len = webRadioHttpClient.getSize();
    _currentStream = (MusicStream *)webRadioHttpClient.getStreamPtr();

    if (_currentStream == NULL)
    {
      Serial.println("Current Stream is null");
      break;
    } 

    while (webRadioHttpClient.connected() && (len > 0 || len == -1) && _currentStream != NULL && !stopPlaying)
    {
      if (overwriteChunking)
        chunked = false;
      long chunkSize = chunked ? _currentStream->GetChunkSize() : 4096;
      if (chunkSize == -1)
      {
        overwriteChunking = true;
        chunkSize = 4096;
      }
      long toRead = chunkSize; // chunkSize;

      while (toRead && !stopPlaying)
      {
        if (circularBuffer.size() < circularBuffer.capacity - toRead)
        {
          mp3buff = new uint8_t[toRead];
          if (!mp3buff)
            Serial.println("Cannot allocate memory to mp3 buffer.");

          int readed = _currentStream->readBytes(mp3buff, toRead);          
          for (int i = 0; i < readed; i++)
          {
            mtx.lock();
            circularBuffer.push(mp3buff[i]);
            mtx.unlock();
          }
          if (mp3buff)
          {
            delete[] mp3buff;
            mp3buff = NULL;
          }
          toRead -= readed;
          _playerState = PLAYING;
        }
        else
        {
          delay(1);
        }
      }
    }
  }
  if (mp3buff)
  {
    delete[] mp3buff;
    mp3buff = NULL;
  }
  _currentStream->stop();
  webRadioHttpClient.end();
  _playerState = STOPPED;
  Serial.println("Ending play task");
  vTaskDelete(playTaskHandle);
}

static void PlayAudioTask(void *parameters)
{
  esp_task_wdt_init(30, false);

  while (true)
  {
    if (circularBuffer.size() > 32)
    {      
      uint8_t audio[32];

      for (int i = 0; i < 32; i++)
      {
        mtx.lock();
        audio[i] = circularBuffer.shift();
        mtx.unlock();
      }
      vs1053.Play(audio, 32);

      if(circularBuffer.size()<128)
      {
         std::string info = "Warning  - Buffer size: " + std::to_string(circularBuffer.size());
        Serial.println(info.c_str());
      }
    }
    else
    {
      if (_playerState == STOPPED)
        delay(100);
      else
      {
        std::string info = "Buffer empty: " + std::to_string(circularBuffer.size());
        Serial.println(info.c_str());             
       
        delay(100);
      }
    }
  }
}

void StartPlayTask(String url)
{
  _currentURL = url;
  circularBuffer.clear();
  xTaskCreate(PlayTask, "PlayTask", 8000, NULL, 3, &playTaskHandle);
}

void StartAudioPlayTask()
{
  xTaskCreate(PlayAudioTask, "PlayAudioTask", 8000, NULL, 3, &playAudioTaskHandle);
}

void StopPlayTask()
{
  stopPlaying = true;
}

void ToggleChunking()
{
  overwriteChunking = !overwriteChunking;
  Serial.println("Toggle chunking");
}

void WaitForState(PlayerState state)
{
  while (_playerState != state)
  {
    delay(5);
  }
}

#endif