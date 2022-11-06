#include <Arduino.h>
#include <esp32HTTPrequest.h>
#include <ArduinoJson.h>
#include <base64.h>

#include "status.h"
#include "config.h"

namespace NFCAlarm
{
    namespace Status
    {
        struct timeval TimeAtBoot;
        bool Connected = false;
        String AlarmState = "Unknown";

        unsigned long previousMillis = 0;

        void setup()
        {
            if (gettimeofday(&TimeAtBoot, NULL) != 0) 
            {
                Serial.println(F("Failed to obtain time"));
            }

        }

        void requestReadyStateChange(void* optParm, esp32HTTPrequest* request, int readyState)
        {
            if (readyState == 4)
            {
                request->setDebug(false);

                String responseText = request->responseText();
                if (request->responseHTTPcode() == 200)
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
                else
                {
                    Serial.printf("Item state retrieval failed: %d\r\n", request->responseHTTPcode());
                    Serial.println(responseText);
                }
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
                    String url = "http://";
                    url += Config::OpenHabServerName;
                    url += "/rest/items/";
                    url += Config::AlarmItemName;

                    esp32HTTPrequest request;

                    request.setDebug(false);
                    request.onReadyStateChange(requestReadyStateChange, nullptr);
                    request.open("GET", url.c_str());
                    request.setReqHeader("accept", "application/json");
                    if (Config::OpenHabAPIKey[0] != 0)
                    {
                        String userId = Config::OpenHabAPIKey;
                        userId += ":";
                        userId = base64::encode(userId);
                        userId = "Basic " + userId;
                        request.setReqHeader("Authorization", userId.c_str());
                    }
                    request.send();
                }
            }
        }
    }
}