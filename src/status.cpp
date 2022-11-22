#include <Arduino.h>
#include <esp32HTTPrequest.h>
#include <ArduinoJson.h>
#include <base64.h>

#include "status.h"
#include "config.h"

#define BELL_PIN 27
#define UNLOCK_PIN 26
#define ALARM_ON_LED_PIN 18
#define ALARM_OFF_LED_PIN 19
#define LED_ON 1
#define LED_OFF 0

namespace NFCAlarm
{
    namespace Status
    {
        struct timeval TimeAtBoot;
        bool Connected = false;
        bool MustRefreshAlarmState = false;
        String AlarmState = "Unknown";

        volatile bool BellAsserted = false;
        void IRAM_ATTR bellPinISR() 
        {
            BellAsserted = (digitalRead(BELL_PIN) == 0);
        }

        volatile bool UnlockAsserted = false;
        void IRAM_ATTR unlockPinISR()
        {
            UnlockAsserted = (digitalRead(UNLOCK_PIN) == 0);
        }

        void setup()
        {
            if (gettimeofday(&TimeAtBoot, NULL) != 0) 
            {
                Serial.println(F("Failed to obtain time"));
            }

            pinMode(BELL_PIN, INPUT_PULLUP);
            ::attachInterrupt(digitalPinToInterrupt(BELL_PIN), &bellPinISR, CHANGE);

            pinMode(UNLOCK_PIN, INPUT_PULLUP);
            ::attachInterrupt(digitalPinToInterrupt(UNLOCK_PIN), &unlockPinISR, CHANGE);

            pinMode(ALARM_ON_LED_PIN, OUTPUT);
            digitalWrite(ALARM_ON_LED_PIN, LED_OFF);

            pinMode(ALARM_OFF_LED_PIN, OUTPUT);
            digitalWrite(ALARM_OFF_LED_PIN, LED_OFF);
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

                if (AlarmState == "ON")
                {
                    digitalWrite(ALARM_ON_LED_PIN, LED_ON);
                    digitalWrite(ALARM_OFF_LED_PIN, LED_OFF);
                }
                else if (AlarmState == "OFF")
                {
                    digitalWrite(ALARM_ON_LED_PIN, LED_OFF);
                    digitalWrite(ALARM_OFF_LED_PIN, LED_ON);
                }
                else
                {
                    digitalWrite(ALARM_ON_LED_PIN, LED_OFF);
                    digitalWrite(ALARM_OFF_LED_PIN, LED_OFF);
                }
            }
        }

        void setAlarmStateCallback(String& responseText)
        {
            MustRefreshAlarmState = true;
        }

        void setAlarmState(const char* state)
        {
            esp32HTTPrequest request;
            prepareRequest(request, "POST", setAlarmStateCallback);
            request.send(state);
        }

        void loop()
        {
            static unsigned long previousMillis = 0;
            if (Connected && ((millis() - previousMillis > Config::RefreshPeriod * 1000) || MustRefreshAlarmState))
            {
                previousMillis = millis();
                MustRefreshAlarmState = false;

                if (Config::OpenHabServerName[0] != 0 && Config::AlarmItemName[0] != 0)
                {
                    Serial.println("Retrieving openHAB item status");

                    esp32HTTPrequest request;
                    prepareRequest(request, "GET", itemRetrievalCallback);
                    request.send();
                }
            }

            static bool previousBellAsserted = false;
            if (BellAsserted ^ previousBellAsserted)
            {
                previousBellAsserted = BellAsserted;

                static unsigned long previousBellAssertedMillis = 0;
                if (millis() - previousBellAssertedMillis > 1000)  // debounce, at least 1 second between presses
                {
                    previousBellAssertedMillis = millis();

                    if (previousBellAsserted)
                    {
                        Serial.println("Bell is pressed, activating alarm");
                        setAlarmState("ON");
                    }
                }
            }

            static bool previousUnlockAsserted = false;
            if (UnlockAsserted ^ previousUnlockAsserted)
            {
                previousUnlockAsserted = UnlockAsserted;
                if (previousUnlockAsserted)
                {
                    Serial.println("Unlock signaled, deactivating alarm");
                    setAlarmState("OFF");
                }
            }
        }
    }
}