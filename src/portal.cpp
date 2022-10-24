#include <IotWebConf.h>

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

        void setup()
        {
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
            // -- Let IotWebConf test and handle captive portal requests.
            if (iotWebConf.handleCaptivePortal())
            {
                // -- Captive portal request were already served.
                return;
            }
            
            String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
            s += "<title>NFCAlarm</title></head><body>";
            s += "Go to <a href='config'>configure page</a> to change settings.";
            s += "</body></html>\n";

            server.send(200, "text/html", s);
        }
   }
}