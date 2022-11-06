#include <Arduino.h>

#include "status.h"

namespace NFCAlarm
{
    namespace Status
    {
        struct timeval TimeAtBoot;

        void setup()
        {
            if (gettimeofday(&TimeAtBoot, NULL) != 0) 
            {
                Serial.println(F("Failed to obtain time"));
            }

        }

        void loop()
        {

        }
    }
}