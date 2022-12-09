/*
 NFCAlarm - The embedded program to connect to a OBO Hands RFID access reader

 The contents of this file are subject to the Mozilla Public License Version 2.0 (the "License");
 you may not use this file except in compliance with the License. You may obtain a copy of the
 License at http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 ANY KIND, either express or implied. See the License for the specific language governing rights
 and limitations under the License.

 The Original Code is main.cpp

 The Initial Developer of the Original Code is Olivier Sannier.
 Portions created by Olivier Sannier are Copyright (C) of Olivier Sannier. All rights reserved.
*/
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