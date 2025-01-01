#ifndef MUSICSTREAM_H
#define MUSICSTREAM_H

#include <WiFiClient.h>

class ChunkedStream : public WiFiClient
{
public:
  long GetChunkSize();
};

long ChunkedStream::GetChunkSize()
{
    constexpr int TIMEOUT_MS = 2000;  
    constexpr size_t BUFFER_SIZE = 16; 
    char buffer[BUFFER_SIZE] = {0};
    long startMillis = millis();
    int index = 0;
       
    while (millis() - startMillis < TIMEOUT_MS)
    {
        if (available())
        {
            char readedChar = (char)read();            
            if (readedChar == '\n' && index > 0 && buffer[index - 1] == '\r')
            {
                buffer[index - 1] = '\0'; 
                long chunkSize = strtol(buffer, nullptr, 16); 

                if (chunkSize < 0)
                    return -1; 
                return chunkSize;
            }            
            if (index < BUFFER_SIZE - 1)
                buffer[index++] = readedChar;
        }
    }    
    return -1;
}

#endif