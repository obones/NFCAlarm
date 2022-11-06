#include "config.h"

namespace NFCAlarm
{
    namespace Config
    {
        char NTPServerName[NTPServerNameLength] = "pool.ntp.org";

        char OpenHabServerName[OpenHabServerNameLength] = "";
        char OpenHabAPIKey[OpenHabAPIKeyLength] = "";
        char AlarmItemName[AlarmItemNameLength] = "";

        int RefreshPeriod = 10;
    }
}
