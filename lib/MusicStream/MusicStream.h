#ifndef MUSICSTREAM_H
#define MUSICSTREAM_H

#include <WiFiClient.h>

class MusicStream : public WiFiClient
{
public:
  long GetChunkSize();
};

long MusicStream::GetChunkSize()
{  
  long startMilis = millis();
  char buffer[8];
  char readedChar;
  int counts = 0;
  while (true)
  {
    readedChar = (char)read();
    if (readedChar == '\n')
    {
      memset(buffer, 0, 8);
      for (int i = 0; i < 8; i++)
      {
        buffer[i] = (char)read();
        if (buffer[i] == '\n')
        {
          long stopMilis = millis();
          long outVal = strtol(buffer, NULL, 16);
       //   Serial.println("Found at: ");
       //   Serial.println(counts);
          if(counts>15000)
            return -1;
          return outVal;
        }
      }
    }
    counts++;
    if(counts>100000)
    {
    //  Serial.println("Counts: ");
    //  Serial.println(counts);
      return -1;
    }
  }
}

#endif