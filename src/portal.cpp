#include <IotWebConf.h>
#include "config.h"

namespace NFCAlarm
{
    namespace Portal
    {
        // -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
        const char thingName[] = "NFCAlarm";

        // -- Initial password to connect to the Thing, when it creates an own Access Point.
        const char wifiInitialApPassword[] = "ESP32NFCAlarm";

        // -- Method declarations.
        void handleRoot();

        DNSServer dnsServer;
        WebServer server(80);

        IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

        auto ntpServerName = iotwebconf::TextParameter("NTP server", "ntpserver", Config::NTPServerName, sizeof(Config::NTPServerName));

        auto openHabGroup = iotwebconf::ParameterGroup("openhab", "openHAB parameters");
        auto openHabServerName = iotwebconf::TextParameter("Server name", "openHabServerName", Config::OpenHabServerName, sizeof(Config::OpenHabServerName));
        auto openHabApiKey = iotwebconf::TextParameter("API Key", "openhabapikey", Config::OpenHabAPIKey, sizeof(Config::OpenHabAPIKey));
        auto alarmItemName = iotwebconf::TextParameter("Alarm item name", "alarmItemName", Config::AlarmItemName, sizeof(Config::AlarmItemName), "Alarm");

        void setup()
        {
            // -- Setting up parameters
            iotWebConf.addSystemParameter(&ntpServerName);

            openHabGroup.addItem(&openHabServerName);
            openHabGroup.addItem(&alarmItemName);

            iotWebConf.addParameterGroup(&openHabGroup);

            // -- Initializing the configuration.
            iotWebConf.init();

            // -- Set up required URL handlers on the web server.
            server.on("/", handleRoot);
            server.on("/config", []{ iotWebConf.handleConfig(); });
            server.onNotFound([](){ iotWebConf.handleNotFound(); });
        }

        void loop()
        {
            iotWebConf.doLoop();
        }

        /**
         * Handle web requests to "/" path.
         */
        void handleRoot()
        {
            extern const uint8_t index_html_start[] asm("_binary_data_index_html_start");

            // -- Let IotWebConf test and handle captive portal requests.
            if (iotWebConf.handleCaptivePortal())
            {
                // -- Captive portal request were already served.
                return;
            }

            server.send(200, "text/html", (char*)&index_html_start[0]);
        }
   }
}