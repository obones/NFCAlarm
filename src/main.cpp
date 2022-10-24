#include <Arduino.h>
#include "portal.h"

void setup() 
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    NFCAlarm::Portal::setup();

    Serial.println("Ready.");
}

void loop() 
{
    // put your main code here, to run repeatedly:
    NFCAlarm::Portal::loop();
}