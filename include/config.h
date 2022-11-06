#ifndef CONFIG_H
#define CONFIG_H
namespace NFCAlarm
{
    namespace Config
    {
        const int NTPServerNameLength = 100;

        const int OpenHabServerNameLength = 100;
        const int OpenHabAPIKeyLength = 256;
        const int AlarmItemNameLength = 20;

        extern char NTPServerName[NTPServerNameLength];

        extern char OpenHabServerName[OpenHabServerNameLength];
        extern char OpenHabAPIKey[OpenHabAPIKeyLength];
        extern char AlarmItemName[AlarmItemNameLength];
    }
}
#endif