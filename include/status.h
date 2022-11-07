#ifndef STATUS_H
#define STATUS_H

#include <time.h>
#include <string>

namespace NFCAlarm
{
    namespace Status
    {
        extern struct timeval TimeAtBoot;
        extern bool Connected;
        extern String AlarmState;

        void setup();

        void loop();
    }
}
#endif