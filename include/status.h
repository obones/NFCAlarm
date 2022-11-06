#ifndef STATUS_H
#define STATUS_H

#include <time.h>

namespace NFCAlarm
{
    namespace Status
    {
        extern struct timeval TimeAtBoot;
        extern bool Connected = false;

        void setup();

        void loop();
    }
}
#endif