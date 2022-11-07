#include <Arduino.h>
#include <esp32HTTPrequest.h>
#include <ArduinoJson.h>
#include <base64.h>

#include "status.h"
#include "config.h"

#define BELL_PIN 27

namespace NFCAlarm
{
    namespace Status
    {
        struct timeval TimeAtBoot;
        bool Connected = false;
        String AlarmState = "Unknown";

        unsigned long previousMillis = 0;
        volatile bool BellAsserted = false;

        void IRAM_ATTR bellPinISR() 
        {
            BellAsserted = (digitalRead(BELL_PIN) != 0);
        }

        void setup()
        {
            if (gettimeofday(&TimeAtBoot, NULL) != 0) 
            {
                Serial.println(F("Failed to obtain time"));
            }

            pinMode(BELL_PIN, INPUT_PULLUP);
            ::attachInterrupt(digitalPinToInterrupt(BELL_PIN), &bellPinISR, CHANGE);
        }

        typedef void (*responseTextCallback)(String&);

        void requestReadyStateChange(void* optParm, esp32HTTPrequest* request, int readyState)
        {
            if (readyState == 4)
            {
                request->setDebug(false);

                String responseText = request->responseText();

                if (request->responseHTTPcode() == 200)
                {
                    if (optParm)
                    {
                        auto callback = (responseTextCallback)optParm;
                        callback(responseText);
                    }
                }
                else
                {
                    Serial.printf("Request failed: %d\r\n", request->responseHTTPcode());
                    Serial.println(responseText);
                }
            }
        }

        void prepareRequest(esp32HTTPrequest& request, const char* method, responseTextCallback callback)
        {
            String url = "http://";
            url += Config::OpenHabServerName;
            url += "/rest/items/";
            url += Config::AlarmItemName;

            request.setDebug(false);
            request.onReadyStateChange(requestReadyStateChange, (void*)callback);
            request.open(method, url.c_str());
            request.setReqHeader("accept", "application/json");
            request.setReqHeader("Content-Type", "text/plain");
            if (Config::OpenHabAPIKey[0] != 0)
            {
                String userId = Config::OpenHabAPIKey;
                userId += ":";
                userId = base64::encode(userId);
                userId = "Basic " + userId;
                request.setReqHeader("Authorization", userId.c_str());
            }
        }

        void itemRetrievalCallback(String& responseText)
        {
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, responseText);
            if (error)
            {
                Serial.print(F("Failed to read response:"));
                Serial.println(error.f_str());
                Serial.println(responseText);
            }
            else
            {
                auto root = doc.as<JsonObject>();
                AlarmState = root["state"].as<String>();
                Serial.print("State is ");
                Serial.println(AlarmState);
            }
        }

        void loop()
        {
            if (Connected && (millis() - previousMillis > Config::RefreshPeriod * 1000))
            {
                previousMillis = millis();

                Serial.println("Retrieving openHAB item status");
                if (Config::OpenHabServerName[0] != 0 && Config::AlarmItemName[0] != 0)
                {
                    esp32HTTPrequest request;
                    prepareRequest(request, "GET", itemRetrievalCallback);
                    request.send();
                }
            }
            
            static bool previousBellAsserted = false;

            if (BellAsserted ^ previousBellAsserted)
            {
                previousBellAsserted = BellAsserted;
                Serial.println("Bell has changed!");

                if (previousBellAsserted)
                {
                    Serial.println("Bell has been pressed, activating alarm");

                    esp32HTTPrequest request;
                    prepareRequest(request, "POST", nullptr);
                    request.send("ON");
                }
            }
        }
    }
}