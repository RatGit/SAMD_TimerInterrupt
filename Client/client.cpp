////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//  PROJECT:      Wireless Sensor Controller                                                                          //
//                                                                                                                    //
//  MODULE:       client.cpp                                                                                          //
//                                                                                                                    //
//  DESCRIPTION:  Low Power LoRa Packet Radio Client. Used for testing Power Utilisation, Comms Contention etc.       //
//                                                                                                                    //
//  AUTHORS:      Keith Willis (keith.willis@vutronix.com)                                                            //
//                                                                                                                    //
//  NOTES:        Hardware Requirements: Mini Ultra Pro board powered by a single cell Li-Ion/Pol 3.7V battery.       //
//                                                                                                                    //
//                Creates a reliable packet radio client with support for pairing, addressing, retries and low power. //
//                                                                                                                    //
//                It is designed to work with the "link-test-server" sketch                                           //
//                                                                                                                    //
//                **Important**: When using the "SerialFlash.sleep()" function, the only function that the serial     //
//                flash chip will then respond to is "SerialFlash.wakeup()".                                          //
//                                                                                                                    //
//                Radio Defaults after init:                                                                          //
//                 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC ON //                           //
//                 Transmitter power is 13dBm, using PA_BOOST.                                                        //
//                 The RFM95 module, which uses the PA_BOOST pin, can set transmitter power from 5 to 23 dBm          //
//                                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              COMMERCIAL IN CONFIDENCE                                              //
//  This material may not in whole or part be copied, stored electronically or communicated to third                  //
//  parties without <YOUR ORGANISATION NAME HERE>'s prior agreement in writing.                                       //
//                                                                                                                    //
//  You may not remove this confidentiality or copyright notice or modify the contents of this file                   //
//  without <YOUR ORGANISATION NAME HERE>'s prior agreement in writing.                                               //
//                                                                                                                    //
//                                      CopyRight 2018 <YOUR ORGANISATION NAME HERE>                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//  FUNCTIONS:                                                                                                        //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     getUID                                                                                             //
//   Description:  This function returns a 64-bit UID read from the 24AA02E64 IC                                      //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* _uidstr                                                                                      //
//   Returns:      uint64_t                                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        It replaces the 24AA02E64 OUI with a custom one, (CLIENT_OUI).                                     //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     alarmMatch                                                                                         //
//   Description:  RTC Alarm Interrupt Handler.                                                                       //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        This function is simply a placeholder.                                                             //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     pair                                                                                               //
//   Description:  This function attempts to pair the Client to a LoRa Master node                                    //
//                 If not in Low Power mode, it wakes up the USB port and sends serial status messages                //
//                 as well as toggling the LED on a successful handshake.                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     sendData                                                                                           //
//   Description:  This function is called once every program loop. It sends a test message to the server and then    //
//                 waits for a reply.                                                                                 //
//                 If not in Low Power mode, it wakes up the USB port and sends serial status messages                //
//                 as well as toggling the LED on a successful handshake.                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     radioHandshake                                                                                     //
//   Description:  This function is called once every program loop. It sends a test mesage to the server and then     //
//                 waits for a reply. It wakes up the USB port and sends serial status messages as well as toggling   //
//                 the LED on a successful handshake.                                                                 //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Called in "loop"                                                                                   //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     setup                                                                                              //
//   Description:  Main sketch initialisation function. Sets unused IO pins to their lowest power mode, (ie. Input    //
//                 + PullUp), puts the Serial Flash to sleep, detaches the USB port, configures the LoRa radio module //
//                 and then puts it to sleep. Finally it configures the RTC to wake once every minute                 //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     loop                                                                                               //
//   Description:  Main sketch program loop. Calls the "radioHandshake" function once every program loop then places  //
//                 the RTC back into StandBy mode.                                                                    //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////
//  Included Modules and Functions  //
//////////////////////////////////////

#include "common.h"
#include "client.h"


//////////////////////////////////////
//  Global Variables and Constants  //
//////////////////////////////////////

uint8_t serverAddress;                                      // Server Address (This will be changed after pairing)
RHReliableDatagram manager(radio, DEFAULT_CLIENT_ADDRESS);  // Class to manage message delivery and receipt, using the driver declared above


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     getUID                                                                                             //
//   Description:  This function returns a 64-bit UID read from the 24AA02E64 IC                                      //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* _uidstr                                                                                      //
//   Returns:      uint64_t                                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        It replaces the 24AA02E64 OUI with a custom one, (CLIENT_OUI).                                     //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t getUID(char* _uidstr)
{
 Wire.begin();
 Wire.beginTransmission(EUI64_CHIP_ADDRESS);
 Wire.write(UID_ADDRESS);
 Wire.endTransmission();
 Wire.requestFrom(EUI64_CHIP_ADDRESS, UID_LENGTH);

 unsigned char buf[UID_LENGTH];

 uint8_t ptr = 0;
 while (Wire.available()) buf[ptr++] = Wire.read(); // Format needs to be little endian (LSB...MSB)

 sprintf(_uidstr, "%s%02X%02X%02X%02X%02X", CLIENT_OUI, buf[0], buf[1], buf[2], buf[3], buf[4]);

 return strtoull(_uidstr, NULL, 16);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     alarmMatch                                                                                         //
//   Description:  RTC Alarm Interrupt Handler.                                                                       //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        This function is simply a placeholder.                                                             //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void alarmMatch()
{
 if (radioInitialised) radioHandshake();  // If Paired, send Valid Data else send a Pairing Request
 rtc.standbyMode();  // Sleep until next alarm match
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     pair                                                                                               //
//   Description:  This function attempts to pair the Client to a LoRa Master node                                    //
//                 If not in Low Power mode, it wakes up the USB port and sends serial status messages                //
//                 as well as toggling the LED on a successful handshake.                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pair()
{
 char* buf;
 uint8_t* data, datalen;

 #ifndef LOW_POWER
  buf = new char[255];
 #endif

 // Pairing Request: <64bit OUI|UID> From DEFAULT_CLIENT_ADDRESS To PAIRING_ADDRESS
 datalen = 16;
 data = new uint8_t[17];
 strcpy((char*)data, uidstr);  // Write 64-bit OUI:UID to datagram
 serverAddress = PAIRING_ADDRESS;

 #ifndef LOW_POWER
  serialPrintf(buf, "CLIENT: [%u > %u]: \"%s\"[%d]", true, (uint8_t)(manager.thisAddress()), serverAddress, (char*)data, datalen);
//  serialPrintf(buf, "Sending Message: \"%s\" Length=%d To=%u From=%u", true, (char*)data, datalen, serverAddress, (uint8_t)(manager.thisAddress()));
 #endif

 if (manager.sendtoWait(data, datalen, serverAddress))  // Send a message to the LoRa Server
 {
  #ifndef LOW_POWER
//   serialPrint((char*)"Message Sent to Server");
   serialPrint((char*)"CLIENT: Sent OK");
   digitalWrite(LED_BUILTIN, HIGH);
   delay(100);
   digitalWrite(LED_BUILTIN, LOW);
  #endif

  // Pairing Request: <64bit OUI|UID> From DEFAULT_CLIENT_ADDRESS To PAIRING_ADDRESS
  uint8_t retries=NUM_RETRIES, len, from;

  bool PairingRequestResponseReceived = false;
  while (!PairingRequestResponseReceived && retries-- > 0)
  {
   len = 22;
   if (manager.recvfromAckTimeout(msgBuffer, &len, SERVER_ACK_TIMEOUT, &from))  // Now wait for a Pairing Request Response from the server: <64bit Client OUI|UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS>
   {
    msgBuffer[22] = 0;

    #ifndef LOW_POWER
     serialPrintf(buf, "Message Received: \"%s\" Length=%u To=%u From=%u",true, (char*)msgBuffer, len, manager.headerTo(), from);
    #endif

    if (len == 22)
    {
     msgBuffer[16] = 0;
     uint64_t clientUID = strtoull((char*)msgBuffer, NULL, 16);  // Convert first 16 hex chars into a 64bit OUI:UID
     PairingRequestResponseReceived = (clientUID == uid);        // Check that the packet's OUI:UID is the same as this client's
    }
   }
  }

  if (PairingRequestResponseReceived)
  {
   #ifndef LOW_POWER
    msgBuffer[16] = ':';
    serialPrintf(buf, "Pairing Request Response Received: \"%s\" Length=%u To=%u From=%u", true, (char*)msgBuffer, len, manager.headerTo(), from);
   #endif

   uint8_t clientAddress = strtoul((char*)(&(msgBuffer[20])), NULL, 16);     // Convert last 2 hex chars into an 8bit Client Address
   msgBuffer[19] = 0;
   uint8_t tempserverAddress = strtoul((char*)(&(msgBuffer[17])), NULL, 16); // Convert previous 2 hex chars into an 8bit Server Address

   manager.setThisAddress(clientAddress);                      // Set the new client address to respond from

   uint8_t response[20];
   sprintf((char*)response, "%s:%02X",  uidstr, clientAddress);  // Create Pairing Request Response Handshake datagram: <64-bit OUI|UID>:<8bit CLIENT ADDRESS>

   #ifndef LOW_POWER
    serialPrintf(buf, "Sending Pairing Request Response Handshake: \"%s\" To: %u", true, (char*)response, PAIRING_ADDRESS);
   #endif

   if (manager.sendtoWait(response, 19, PAIRING_ADDRESS))  // Send Pairing Response Handshake: <64bit OUI:UID><8bit CLIENT ADDRESS> From CLIENT_ADDRESS To PAIRING_ADDRESS
   {
    #ifndef LOW_POWER
     serialPrint((char*)"Waiting for Pairing Request Response Handshake Acknowledgment");
    #endif

    retries=NUM_RETRIES;
    bool PairingRequestResponseHandshakeAcknowledgementReceived = false;
    while (!PairingRequestResponseHandshakeAcknowledgementReceived && retries-- > 0)
    {
     len = 33;
     if (manager.recvfromAckTimeout(msgBuffer, &len, SERVER_ACK_TIMEOUT, &from))  // Now wait for a Pairing Request Response Handshake Acknowledgement from the server: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS>:<Timestamp>
     {
      msgBuffer[33] = 0;

      #ifndef LOW_POWER
       serialPrintf(buf, "Message Received: \"%s\" Length=%u To=%u From=%u", true, (char*)msgBuffer, len, manager.headerTo(), from);
      #endif

      if (len == 33)
      {
       uint32_t serverTimestamp = strtoul((char*)(&(msgBuffer[23])), NULL, 10);    // Convert last 10 decimal chars into a 32-bit timestamp
       msgBuffer[22] = 0;
       uint8_t newClientAddress = strtoul((char*)(&(msgBuffer[20])), NULL, 16);    // Convert previous 2 hex chars into an 8-bit Client Address
       msgBuffer[19] = 0;
       uint8_t newServerAddress = strtoul((char*)(&(msgBuffer[17])), NULL, 16);    // Convert previous 2 hex chars into an 8-it Server Address
       msgBuffer[16] = 0;
       uint64_t clientUID2 = strtoull((char*)msgBuffer, NULL, 16);                 // Convert first 16 hex chars into a 64-bit OUI:UID

       PairingRequestResponseHandshakeAcknowledgementReceived = (clientUID2 == uid && newServerAddress == tempserverAddress && newClientAddress == clientAddress);  // Check that the packet's OUI:UID is the same as this client's
       if (PairingRequestResponseHandshakeAcknowledgementReceived)
       {
        rtc.setY2kEpoch(serverTimestamp);
       }
      }
     }
    }

    if (PairingRequestResponseHandshakeAcknowledgementReceived)
    {
     serverAddress = tempserverAddress;  // Permanently set Server Address

     #ifndef LOW_POWER
      msgBuffer[16] = ':';
      msgBuffer[19] = ':';
      serialPrintf(buf, "Pairing Request Response Handshake Acknowledgment Validated: Server Address=%u : This Address=%u", true, serverAddress, clientAddress);
     #endif

     isPaired = true;
    }
    else
    {
     manager.setThisAddress(DEFAULT_CLIENT_ADDRESS);  // If pairing fails then reset Client Address to the DEFAULT_CLIENT_ADDRESS
    }
   }
   else
   {
    manager.setThisAddress(DEFAULT_CLIENT_ADDRESS);   // If pairing fails then reset Client Address to the DEFAULT_CLIENT_ADDRESS
   }
  }
  else
  {
   #ifndef LOW_POWER
    serialPrint((char*)"Handshake not Received");
   #endif
  }
 }
 else
 {
  #ifndef LOW_POWER
   serialPrint((char*)"No ACK from Server");
  #endif
 }

 #ifndef LOW_POWER
  delete [] buf;
 #endif

 delete [] data;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     sendData                                                                                           //
//   Description:  This function is called once every program loop. It sends a test message to the server and then    //
//                 waits for a reply.                                                                                 //
//                 If not in Low Power mode, it wakes up the USB port and sends serial status messages                //
//                 as well as toggling the LED on a successful handshake.                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sendData()
{
 char* buf;
 uint8_t* data, datalen;

 #ifndef LOW_POWER
  buf = new char[255];
 #endif

 // Send Data: <64bit OUI|UID>:<12-bit Data>:<12-bit Data>
 datalen = 24;
 data = new uint8_t[25];
 strcpy((char*)data, uidstr);                            // Write 64-bit OUI|UID to datagram
 sprintf((char*)(&(data[16])), ":%03X", rand() % 4096);  // Random integer in the range 0 to 4095, (simulate 12-bit sensor data)
 sprintf((char*)(&(data[20])), ":%03X", rand() % 4096);  // Random integer in the range 0 to 4095, (simulate 12-bit sensor data)

 #ifndef LOW_POWER
  serialPrintf(buf, "CLIENT: [%u > %u]: \"%s\"[%d]", true, (uint8_t)(manager.thisAddress()), serverAddress, (char*)data, datalen);
//  serialPrintf(buf, "Sending Message: \"%s\" Length=%d To=%u From=%u", true, (char*)data, datalen, serverAddress, (uint8_t)(manager.thisAddress()));
 #endif

 if (manager.sendtoWait(data, datalen, serverAddress))  // Send a message to the LoRa Server
 {
  #ifndef LOW_POWER
//   serialPrint((char*)"Message Sent to Server");
   serialPrint((char*)"CLIENT: Sent OK");
   digitalWrite(LED_BUILTIN, HIGH);
   delay(100);
   digitalWrite(LED_BUILTIN, LOW);
  #endif

 // Wait for a response from the server with a new timestamp, (maybe not required for concrete sensor)
 }
 else
 {
  #ifndef LOW_POWER
   serialPrint((char*)"No ACK from Server");
  #endif
 }

 #ifndef LOW_POWER
  delete [] buf;
 #endif

 delete [] data;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     radioHandshake                                                                                     //
//   Description:  This function is called once every program loop. If unpaired it will initiate a pairing request,   //
//                 otherwise it sends data to the server and then waits for a reply.                                  //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Called in "loop"                                                                                   //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void radioHandshake()
{
 if (isPaired) sendData();
 else pair();

 #ifdef LOW_POWER
  radio.sleep();
 #else
  serialPrint((char*)"");
 #endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     radioHandshake                                                                                     //
//   Description:  This function is called once every program loop. It sends a test message to the server and then    //
//                 waits for a reply. It wakes up the USB port and sends serial status messages as well as toggling   //
//                 the LED on a successful handshake.                                                                 //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    bool pairing                                                                                       //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Called in "loop"                                                                                   //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void oldradioHandshake(bool pairing=false)
{
 char* buf;
 uint8_t* data, datalen;

 #ifndef LOW_POWER
  buf = new char[255];
 #endif

 if (pairing)  // Pairing Request: <64bit OUI|UID> From DEFAULT_CLIENT_ADDRESS To PAIRING_ADDRESS
 {
  datalen = 16;
  data = new uint8_t[17];
  strcpy((char*)data, uidstr);  // Write 64-bit OUI:UID to datagram
  serverAddress = PAIRING_ADDRESS;
 }
 else  // Send Data: <64bit OUI|UID>:<12-bit Data>:<12-bit Data>
 {
  datalen = 24;
  data = new uint8_t[25];
  strcpy((char*)data, uidstr);                            // Write 64-bit OUI|UID to datagram
  sprintf((char*)(&(data[16])), ":%03X", rand() % 4096);  // Random integer in the range 0 to 4095, (simulate 12-bit sensor data)
  sprintf((char*)(&(data[20])), ":%03X", rand() % 4096);  // Random integer in the range 0 to 4095, (simulate 12-bit sensor data)
 }

 #ifndef LOW_POWER
  serialPrintf(buf, "CLIENT: [%u > %u]: \"%s\"[%d]", true, (uint8_t)(manager.thisAddress()), serverAddress, (char*)data, datalen);
//  serialPrintf(buf, "Sending Message: \"%s\" Length=%d To=%u From=%u", true, (char*)data, datalen, serverAddress, (uint8_t)(manager.thisAddress()));
 #endif

 if (manager.sendtoWait(data, datalen, serverAddress))  // Send a message to the LoRa Server
 {
  #ifndef LOW_POWER
//   serialPrint((char*)"Message Sent to Server");
   serialPrint((char*)"CLIENT: Sent OK");
   digitalWrite(LED_BUILTIN, HIGH);
   delay(100);
   digitalWrite(LED_BUILTIN, LOW);
  #endif

  if (pairing)  // Pairing Request: <64bit OUI|UID> From DEFAULT_CLIENT_ADDRESS To PAIRING_ADDRESS
  {
   uint8_t retries=NUM_RETRIES, len, from;

   bool PairingRequestResponseReceived = false;
   while (!PairingRequestResponseReceived && retries-- > 0)
   {
    len = 22;
    if (manager.recvfromAckTimeout(msgBuffer, &len, SERVER_ACK_TIMEOUT, &from))  // Now wait for a Pairing Request Response from the server: <64bit Client OUI|UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS>
    {
     msgBuffer[22] = 0;

     #ifndef LOW_POWER
      serialPrintf(buf, "Message Received: \"%s\" Length=%u To=%u From=%u",true, (char*)msgBuffer, len, manager.headerTo(), from);
     #endif

     if (len == 22)
     {
      msgBuffer[16] = 0;
      uint64_t clientUID = strtoull((char*)msgBuffer, NULL, 16);  // Convert first 16 hex chars into a 64bit OUI:UID
      PairingRequestResponseReceived = (clientUID == uid);        // Check that the packet's OUI:UID is the same as this client's
     }
    }
   }

   if (PairingRequestResponseReceived)
   {
    #ifndef LOW_POWER
     msgBuffer[16] = ':';
     serialPrintf(buf, "Pairing Request Response Received: \"%s\" Length=%u To=%u From=%u", true, (char*)msgBuffer, len, manager.headerTo(), from);
    #endif

    uint8_t clientAddress = strtoul((char*)(&(msgBuffer[20])), NULL, 16);     // Convert last 2 hex chars into an 8bit Client Address
    msgBuffer[19] = 0;
    uint8_t tempserverAddress = strtoul((char*)(&(msgBuffer[17])), NULL, 16); // Convert previous 2 hex chars into an 8bit Server Address

    manager.setThisAddress(clientAddress);                      // Set the new client address to respond from

    uint8_t response[20];
    sprintf((char*)response, "%s:%02X",  uidstr, clientAddress);  // Create Pairing Request Response Handshake datagram: <64-bit OUI|UID>:<8bit CLIENT ADDRESS>

    #ifndef LOW_POWER
     serialPrintf(buf, "Sending Pairing Request Response Handshake: \"%s\" To: %u", true, (char*)response, PAIRING_ADDRESS);
    #endif

    if (manager.sendtoWait(response, 19, PAIRING_ADDRESS))  // Send Pairing Response Handshake: <64bit OUI:UID><8bit CLIENT ADDRESS> From CLIENT_ADDRESS To PAIRING_ADDRESS
    {
     #ifndef LOW_POWER
      serialPrint((char*)"Waiting for Pairing Request Response Handshake Acknowledgment");
     #endif

     retries=NUM_RETRIES;
     bool PairingRequestResponseHandshakeAcknowledgementReceived = false;
     while (!PairingRequestResponseHandshakeAcknowledgementReceived && retries-- > 0)
     {
      len = 33;
      if (manager.recvfromAckTimeout(msgBuffer, &len, SERVER_ACK_TIMEOUT, &from))  // Now wait for a Pairing Request Response Handshake Acknowledgement from the server: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS>:<Timestamp>
      {
       msgBuffer[33] = 0;

       #ifndef LOW_POWER
        serialPrintf(buf, "Message Received: \"%s\" Length=%u To=%u From=%u", true, (char*)msgBuffer, len, manager.headerTo(), from);
       #endif

       if (len == 33)
       {
        uint32_t serverTimestamp = strtoul((char*)(&(msgBuffer[23])), NULL, 10);    // Convert last 10 decimal chars into a 32-bit timestamp
        msgBuffer[22] = 0;
        uint8_t newClientAddress = strtoul((char*)(&(msgBuffer[20])), NULL, 16);    // Convert previous 2 hex chars into an 8-bit Client Address
        msgBuffer[19] = 0;
        uint8_t newServerAddress = strtoul((char*)(&(msgBuffer[17])), NULL, 16);    // Convert previous 2 hex chars into an 8-it Server Address
        msgBuffer[16] = 0;
        uint64_t clientUID2 = strtoull((char*)msgBuffer, NULL, 16);                 // Convert first 16 hex chars into a 64-bit OUI:UID

        PairingRequestResponseHandshakeAcknowledgementReceived = (clientUID2 == uid && newServerAddress == tempserverAddress && newClientAddress == clientAddress);  // Check that the packet's OUI:UID is the same as this client's
        if (PairingRequestResponseHandshakeAcknowledgementReceived)
        {
         rtc.setY2kEpoch(serverTimestamp);
        }
       }
      }
     }

     if (PairingRequestResponseHandshakeAcknowledgementReceived)
     {
      serverAddress = tempserverAddress;  // Permanently set Server Address

      #ifndef LOW_POWER
       msgBuffer[16] = ':';
       msgBuffer[19] = ':';
       serialPrintf(buf, "Pairing Request Response Handshake Acknowledgment Validated: Server Address=%u : This Address=%u", true, serverAddress, clientAddress);
      #endif

      isPaired = true;
     }
     else
     {
      manager.setThisAddress(DEFAULT_CLIENT_ADDRESS);  // If pairing fails then reset Client Address to the DEFAULT_CLIENT_ADDRESS
     }
    }
    else
    {
     manager.setThisAddress(DEFAULT_CLIENT_ADDRESS);   // If pairing fails then reset Client Address to the DEFAULT_CLIENT_ADDRESS
    }
   }
   else
   {
    #ifndef LOW_POWER
     serialPrint((char*)"Handshake not Received");
    #endif
   }
  }
  else
  {
   // Wait for a response from the server with a new timestamp, (maybe not required for concrete sensor)
  }
 }
 else
 {
  #ifndef LOW_POWER
   serialPrint((char*)"No ACK from Server");
  #endif
 }

 #ifndef LOW_POWER
  delete [] buf;
 #endif

 delete [] data;

 #ifdef LOW_POWER
  radio.sleep();
 #else
  serialPrint((char*)"");
 #endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     setup                                                                                              //
//   Description:  Main sketch initialisation function. Sets unused IO pins to their lowest power mode, (ie. Input    //
//                 + PullUp), puts the Serial Flash to sleep, detaches the USB port, configures the LoRa radio module //
//                 and then puts it to sleep. Finally it configures the RTC to wake once every minute                 //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
// uint32_t pinNumber;

 // Switch unused pins as input and enabled built-in pullup
// for (pinNumber = 0; pinNumber < 23; pinNumber++) pinMode(pinNumber, INPUT_PULLUP);
// for (pinNumber = 32; pinNumber < 42; pinNumber++) pinMode(pinNumber, INPUT_PULLUP);

// pinMode(25, INPUT_PULLUP);
// pinMode(26, INPUT_PULLUP);

 // Tri-State GPIO pin incorrectly connected to MISO net
 pinMode(30, INPUT);

 // Tri-State Unused Radio Pins
 pinMode(radioReset, INPUT);
 pinMode(radioDio1, INPUT);
 pinMode(radioDio2, INPUT);

 // Ensure serial flash is not interfering with radio communication on SPI bus
 pinMode(flashChipSelect, OUTPUT);
 digitalWrite(flashChipSelect, HIGH);

 // Ensure radio communication is not interfering with serial flash on SPI bus
 pinMode(radioChipSelect, OUTPUT);
 digitalWrite(radioChipSelect, HIGH);

 // Turn Off LED
 pinMode(LED_BUILTIN, OUTPUT);
 digitalWrite(LED_BUILTIN, HIGH);

 uid = getUID(uidstr);

 serverAddress = DEFAULT_SERVER_ADDRESS;

 #ifndef LOW_POWER
  Serial.begin(BAUD_RATE);
//  while (!Serial) ; // Wait for serial port to be available (This will cause it to hang when not connected to a USB serial port)
 #endif

 radioInitialised = manager.init();
 if (!radioInitialised)
 {
  #ifndef LOW_POWER
   serialPrint((char*)"Radio inititialisation failed");
  #endif
 }
 else
 {
  radio.setFrequency(RADIO_FREQ);
  radio.setTxPower(RADIO_POWER, false);
//  radio.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
//  radio.setCADTimeout(10000);  // You can optionally require this module to wait until Channel Activity Detection shows no activity on the channel before transmitting by setting the CAD timeout to non-zero:

//  manager.setTimeout(1000);
//  manager.setRetries(5);
 }
 #ifdef LOW_POWER
  radio.sleep();
 #endif

 SerialFlash.begin(flashChipSelect);
 SerialFlash.sleep();

 #ifdef LOW_POWER
  USBDevice.detach();
 #endif

 // ***** IMPORTANT 15 SEC DELAY FOR CODE UPLOAD BEFORE USB PORT DETACH DURING SLEEP (LOW POWER MODE ONLY) *****
 #ifdef LOW_POWER
  delay(15000);
 #else
  delay(5000);  // 5 second delay so I have time to capture the pairing request result after startup
 #endif

 // Change these values to set the current initial date and time
 dateTime.second = 0;  // 0 - 59
 dateTime.minute = 0;  // 0 - 59
 dateTime.hour = 0;    // 0 - 23
 dateTime.day = 0;     // 0 - 6, Sunday is day 0
 dateTime.month = 0;   // 1 - 12
 dateTime.year = 0;    // 4 digit year

 // RTC initialization
 rtc.begin();
 rtc.setTime(dateTime.hour, dateTime.minute, dateTime.second);
 rtc.setDate(dateTime.day, dateTime.month, dateTime.year);

 // RTC alarm setting on the 0th second of every minute, (resulting in a 1 minute sleep period)
 #ifdef LOW_POWER
  rtc.setAlarmSeconds(0);
  rtc.enableAlarm(rtc.MATCH_SS);
  rtc.attachInterrupt(alarmMatch);
 #endif

 digitalWrite(LED_BUILTIN, LOW);

 #ifdef LOW_POWER
  rtc.standbyMode();  // Sleep until next alarm match
 #else
  serialPrint((char*)"Starting Client\n");
 #endif

 isPaired = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     loop                                                                                               //
//   Description:  Main sketch program loop. If not in Low Power mode calls the "radioHandshake" function once every  //
//                 program loop otherwise it simply waits.                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
 #ifdef LOW_POWER
  delay(LOOP_DELAY);
 #else
  if (radioInitialised) radioHandshake();  // If Paired, send Valid Data else send a Pairing Request
  delay(LOOP_DELAY);
 #endif
}
