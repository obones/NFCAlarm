#include <Arduino.h>
#include "portal.h"
#include "network.h"
#include "status.h"

void setup() 
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    NFCAlarm::Portal::setup();
    NFCAlarm::Network::setup();
    NFCAlarm::Status::setup();

    Serial.println("Ready.");
}

void loop() 
{
    // put your main code here, to run repeatedly:
    NFCAlarm::Portal::loop();
    NFCAlarm::Status::loop();
}