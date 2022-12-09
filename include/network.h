/*
 NFCAlarm - The embedded program to connect to a OBO Hands RFID access reader

 The contents of this file are subject to the Mozilla Public License Version 2.0 (the "License");
 you may not use this file except in compliance with the License. You may obtain a copy of the
 License at http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 ANY KIND, either express or implied. See the License for the specific language governing rights
 and limitations under the License.

 The Original Code is network.h

 The Initial Developer of the Original Code is Olivier Sannier.
 Portions created by Olivier Sannier are Copyright (C) of Olivier Sannier. All rights reserved.
*/
#ifndef NETWORK_H
#define NETWORK_H
namespace NFCAlarm
{
    namespace Network
    {
        void setup();

        void reconnectServices();
    }
}
#endif