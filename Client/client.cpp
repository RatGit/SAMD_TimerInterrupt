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
//   Function:     setAlarm                                                                                           //
//   Description:  Set next RTC Alarm Time.                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    bool randomise                                                                                     //
//                 uint8_t seconds                                                                                    //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Enable RTC alarm for next minutes and seconds match                                                //
//                 RTC optionally alarms on a random number of seconds, (0-59) at least 1 minute from now             //
//                 in case there was a comms collision                                                                //
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
//   Function:     pinStr                                                                                             //
//   Description:  Set the IO Pin Drive Strength                                                                      //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Works like pinMode(), but to set drive strength                                                    //
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
SPIFlash SerialFlash(flashChipSelect);
RHReliableDatagram manager(radio, DEFAULT_CLIENT_ADDRESS);  // Class to manage message delivery and receipt, using the driver declared above

bool alarmed;            // Global flag to indicate that the RTC wakeup has triggered
bool serialFlashOk;      // Boolean Flag to indicate if Serial Flash was properly initialised
uint8_t pairingCounter;  // This counter is used on startup to count the initial pairing attempts every 10 seconds

Adafruit_Si7021 Si7021 = Adafruit_Si7021();  // Class to manage the Si7021 Temperature/Humidity Sensor


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
 alarmed = true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     setAlarm                                                                                           //
//   Description:  Set next RTC Alarm Time.                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    bool randomise                                                                                     //
//                 uint8_t seconds                                                                                    //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Enable RTC alarm for next minutes and seconds match                                                //
//                 RTC optionally alarms on a random number of seconds, (0-59) at least CAP_CHARGE_DELAY_MINUTES      //
//                 minutes from now                                                                                   //
//                 in case there was a comms collision                                                                //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setAlarm(bool randomise, uint8_t seconds)
{
 int nextAlarmSeconds, nextAlarmMinutes;
 int alarmSeconds, alarmMinutes;

 alarmSeconds = rtc.getSeconds();  // Read current RTC seconds

 if (seconds > 0)
 {
  if (seconds > 60)  // RTC alarms at least CAP_CHARGE_DELAY_MINUTES plus an arbitrary number of seconds, (> 60) into the future. (This option is currently unused in the firmware)
  {
   alarmMinutes = rtc.getMinutes();  // Read current RTC minutes
   int nextAlarmTime = seconds + (alarmMinutes + CAP_CHARGE_DELAY_MINUTES) * 60 + alarmSeconds;

   nextAlarmSeconds = nextAlarmTime % 60;
   nextAlarmMinutes = ((nextAlarmTime - nextAlarmSeconds) / 60) % 60;

   rtc.setAlarmSeconds((uint8_t)nextAlarmSeconds);
   rtc.setAlarmMinutes((uint8_t)nextAlarmMinutes);

   rtc.enableAlarm(rtc.MATCH_MMSS);  // Enable RTC alarm for next minutes and seconds match
  }
  else
  {
   rtc.setAlarmSeconds((alarmSeconds + seconds) % 60);  // RTC alarms in specified second's time
   rtc.enableAlarm(rtc.MATCH_SS);                       // Enable RTC alarm for next seconds match
  }
 }
 else
 {
  alarmMinutes = rtc.getMinutes();  // Read current RTC minutes

  if (randomise)
  {
   srand(time(0));  // Initialize random number generator.
   int nextAlarmTime = (rand() % 60) + (alarmMinutes + CAP_CHARGE_DELAY_MINUTES) * 60 + alarmSeconds;  // RTC alarms in "CAP_CHARGE_DELAY_MINUTES" plus some random number of seconds, (< 60) from now in case there was a comms collision
   nextAlarmSeconds = nextAlarmTime % 60;
   nextAlarmMinutes = ((nextAlarmTime - nextAlarmSeconds) / 60) % 60;
  }
  else
  {
   nextAlarmSeconds = alarmSeconds;                                    // RTC alarms on the current seconds value henceforth
   nextAlarmMinutes = (alarmMinutes + CAP_CHARGE_DELAY_MINUTES) % 60;  // RTC alarms in "CAP_CHARGE_DELAY_MINUTES" time
  }

  rtc.setAlarmMinutes((uint8_t)nextAlarmMinutes);
  rtc.setAlarmSeconds((uint8_t)nextAlarmSeconds);  // RTC alarms on a random number of seconds, (0-59) at least 1 minute from now in case there was a comms collision

  rtc.enableAlarm(rtc.MATCH_MMSS);                 // Enable RTC alarm for next minutes and seconds match
 }
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

bool pair()
{
 uint8_t* data, datalen;

 // Pairing Request: <64bit OUI|UID> From DEFAULT_CLIENT_ADDRESS To PAIRING_ADDRESS
 datalen = 16;
 data = new uint8_t[17];
 strcpy((char*)data, uidstr);  // Write 64-bit OUI:UID to datagram
 serverAddress = PAIRING_ADDRESS;

// #ifndef LOW_POWER
  if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: [%u > %u]: \"%s\"[%d]", true, false, (uint8_t)(manager.thisAddress()), serverAddress, (char*)data, datalen);}
//  if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "Sending Message: \"%s\" Length=%d To=%u From=%u", true, false, (char*)data, datalen, serverAddress, (uint8_t)(manager.thisAddress()));}
// #endif

 if (manager.sendtoWait(data, datalen, serverAddress, SEND_TIMEOUT))  // Send a message to the LoRa Server
// if (manager.sendtoWait(data, datalen, serverAddress))  // Send a message to the LoRa Server
 {
//  #ifndef LOW_POWER
//   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"Message Sent to Server");}
   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Sent OK");}
//  #endif

  #ifndef LOW_POWER
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

//    #ifndef LOW_POWER
     if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: Message Received: \"%s\" Length=%u To=%u From=%u", true, false, (char*)msgBuffer, len, manager.headerTo(), from);}
//    #endif

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
//   #ifndef LOW_POWER
    msgBuffer[16] = ':';
    if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: Pairing Request Response Received: \"%s\" Length=%u To=%u From=%u", true, false, (char*)msgBuffer, len, manager.headerTo(), from);}
//   #endif

   uint8_t clientAddress = strtoul((char*)(&(msgBuffer[20])), NULL, 16);     // Convert last 2 hex chars into an 8bit Client Address
   if (clientAddress == 255)
   {
    if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Pairing Failed");}
   }
   else
   {
    msgBuffer[19] = 0;
    uint8_t tempserverAddress = strtoul((char*)(&(msgBuffer[17])), NULL, 16); // Convert previous 2 hex chars into an 8bit Server Address

    manager.setThisAddress(clientAddress);                      // Set the new client address to respond from

    uint8_t response[20];
    sprintf((char*)response, "%s:%02X",  uidstr, clientAddress);  // Create Pairing Request Response Handshake datagram: <64-bit OUI|UID>:<8bit CLIENT ADDRESS>

//    #ifndef LOW_POWER
     if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: Sending Pairing Request Response Handshake: \"%s\" To: %u", true, false, (char*)response, PAIRING_ADDRESS);}
//    #endif

    if (manager.sendtoWait(response, 19, PAIRING_ADDRESS, SEND_TIMEOUT))  // Send Pairing Response Handshake: <64bit OUI:UID><8bit CLIENT ADDRESS> From CLIENT_ADDRESS To PAIRING_ADDRESS
//    if (manager.sendtoWait(response, 19, PAIRING_ADDRESS))  // Send Pairing Response Handshake: <64bit OUI:UID><8bit CLIENT ADDRESS> From CLIENT_ADDRESS To PAIRING_ADDRESS
    {
//     #ifndef LOW_POWER
      if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Waiting for Pairing Request Response Handshake Acknowledgment");}
//     #endif

     retries=NUM_RETRIES;
     bool PairingRequestResponseHandshakeAcknowledgementReceived = false;
     while (!PairingRequestResponseHandshakeAcknowledgementReceived && retries-- > 0)
     {
      len = 33;
      if (manager.recvfromAckTimeout(msgBuffer, &len, SERVER_ACK_TIMEOUT, &from))  // Now wait for a Pairing Request Response Handshake Acknowledgment from the server: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS>:<Timestamp>
      {
       msgBuffer[33] = 0;

//       #ifndef LOW_POWER
        if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: Message Received: \"%s\" Length=%u To=%u From=%u", true, false, (char*)msgBuffer, len, manager.headerTo(), from);}
//       #endif

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

//      #ifndef LOW_POWER
//       msgBuffer[16] = ':';
//       msgBuffer[19] = ':';
       if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: Pairing Request Response Handshake Acknowledgment Validated: Server Address=%u : This Address=%u", true, false, serverAddress, clientAddress);}
//      #endif

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
  }
  else
  {
//   #ifndef LOW_POWER
    if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Handshake not Received");}
//   #endif
  }
 }
 else
 {
//  #ifndef LOW_POWER
   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: No ACK from Server");}
//  #endif
 }

 delete [] data;

 return isPaired;
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

bool sendData()
{
 bool result = false;
 uint8_t* data, datalen;

 // Send Data: <64bit OUI|UID>:<12-bit Data>:<12-bit Data>
 datalen = PACKET_LENGTH;
 data = new uint8_t[PACKET_LENGTH+1];
 strcpy((char*)data, uidstr);                             // Write 64-bit OUI|UID to datagram

 #ifdef USE_Si7021
  float temperature = Si7021.readTemperature();
  float humidity = Si7021.readHumidity();
  if (temperature == NAN || humidity == NAN) return false;
 #else
  float temperature = (((float)rand()) / (float)RAND_MAX) * 100.0;  // Random Float in the range 0 to 100.0, (simulate 12-bit Si7021 sensor data)
  float humidity = (((float)rand()) / (float)RAND_MAX) * 100.0;     // Random Float in the range 0 to 100.0, (simulate 12-bit Si7021 sensor data)
 #endif

 sprintf((char*)(&(data[16])), ":%06.2f", temperature);  // Zero-Padded 6 character wide float
 sprintf((char*)(&(data[23])), ":%06.2f:", humidity);    // Zero-Padded 6 character wide float

 sprintf((char*)(&(data[31])), "%02X", crc((void*)data, PACKET_LENGTH-2));  // Data Packet: <OUI><UID>:<TEMP>:<RH>:<CRC>  eg. "0004A30B001A531C:123.45:123.45:7E"

// #ifndef LOW_POWER
  if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: [%u > %u]: \"%s\"[%d]", true, false, (uint8_t)(manager.thisAddress()), serverAddress, (char*)data, datalen);}
//  if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "Sending Message: \"%s\" Length=%d To=%u From=%u", true, false, (char*)data, datalen, serverAddress, (uint8_t)(manager.thisAddress()));}
// #endif

 if (manager.sendtoWait(data, datalen, serverAddress, SEND_TIMEOUT))  // Send a message to the LoRa Server
// if (manager.sendtoWait(data, datalen, serverAddress))  // Send a message to the LoRa Server
 {
//  #ifndef LOW_POWER
//   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"Message Sent to Server");}
   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Sent OK");}
//  #endif

  #ifndef LOW_POWER
   digitalWrite(LED_BUILTIN, HIGH);
   delay(100);
   digitalWrite(LED_BUILTIN, LOW);
  #endif

 // Wait for a response from the server with a new timestamp, (maybe not required for concrete sensor)

  result = true;
 }
 else
 {
//  #ifndef LOW_POWER
   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: No ACK from Server");}
//  #endif
 }

 delete [] data;

 return result;
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

bool radioHandshake()
{
 bool result = false;

 if (LED_DEBUG)  // Blink LED, (for debugging only)
 {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
 }

 if (isPaired) result = sendData();
 else result = pair();

// #ifndef LOW_POWER
  if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"");}
// #endif

 return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     pinStr                                                                                             //
//   Description:  Set the IO Pin Drive Strength                                                                      //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Works like pinMode(), but to set drive strength                                                    //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pinStr(uint32_t ulPin, unsigned strength)
{
 if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) {return;}  // Handle the case the pin isn't usable as PIO
 if (strength) strength = 1;      // set drive strength to either 0 or 1 copied
 PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].bit.DRVSTR = strength;
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
 uint32_t pinNumber;

 // Switch unused pins as input and enabled built-in pullup
 for (pinNumber = 0; pinNumber < 23; pinNumber++) pinMode(pinNumber, INPUT_PULLUP);
 for (pinNumber = 32; pinNumber < 42; pinNumber++) pinMode(pinNumber, INPUT_PULLUP);

 pinMode(25, INPUT_PULLUP);
 pinMode(26, INPUT_PULLUP);

 // Tri-State GPIO pin incorrectly connected to MISO net
 pinMode(30, INPUT);

 // Tri-State Power OFF GPIO pin, (it remains a high impedance inputs until it needs to be used as an output)
 pinMode(POWER_OFF_PIN, INPUT);

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
 digitalWrite(LED_BUILTIN, LOW);

 if (USE_SERIAL)
 {
  Serial.begin(BAUD_RATE);
  delay(15000);     // Wait 15 seconds to allow enough time to connect a serial monitor and capture the pairing request result after startup
 }

 uid = getUID(uidstr);

 #ifdef USE_Si7021
//  if (Si7021.begin())
//  {
//   Si7021.getModel();
//   Si7021.getRevision();
//  }
//  else
  if (!Si7021.begin())
  {
   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Si7021 inititialisation failed");}
  }
 #endif

 serverAddress = DEFAULT_SERVER_ADDRESS;

 isPaired = false;
 alarmed = true;

 pairingCounter = 0;  // This counter is used on startup to count the initial pairing attempts every 10 seconds

 manager.setRetries(0);  // Prevent excessive delays waiting for acknowledgments by disabling retries. (This will prevent the super capacitor being drained)
 manager.setTimeout(200);
 radioInitialised = manager.init();
 if (!radioInitialised)
 {
//  #ifndef LOW_POWER
   if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Radio inititialisation failed");}
//  #endif
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
// #ifdef LOW_POWER
//  radio.sleep();
// #endif

 // Serial Flash Initialisation, (also initialises SPI)
 #ifdef USE_FLASH
  if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, "CLIENT: Initialising Serial Flash", true);}
  serialFlashOk = SerialFlash.begin(flashChipSelect);
  if (!serialFlashOk) {if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, "CLIENT: Failed to Initialise Serial Flash", true);} else {serialPrint(USE_SERIAL, ERROR_FLASH_MEMORY, true, true);}}
  SerialFlash.powerDown();
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

 #ifdef LOW_POWER
  rtc.disableAlarm();  // No RTC Alarms until after the first pairing attempt
  rtc.attachInterrupt(alarmMatch);
 #else
  digitalWrite(LED_BUILTIN, HIGH);  // Turn ON LED
 #endif

 if (ENABLE_VERBOSE) {serialPrint(USE_SERIAL, (char*)"CLIENT: Starting Client\n");}
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
  if (alarmed)  // RTC has triggered
  {
   alarmed = false;

   #ifdef TEST_CURRENT
    setAlarm(true);     // RTC alarms in 1 minute's time
    USBDevice.detach(); // Detach USB port
    radio.sleep();      // Put Radio back to sleep
    rtc.standbyMode();  // Sleep until next alarm match
   #else
    if (USE_SERIAL) USBDevice.attach();

    if (radioInitialised)
    {
     if (!isPaired)
     {
      if (pairingCounter < NUM_PAIRING_ATTEMPTS)  // Number of times to attempt to pair every 10 seconds, (on startup)
      {
       pairingCounter++;

       unsigned long time;
       if (ENABLE_VERBOSE) {time = micros();}
       radioHandshake();  // Attempt to Pair with Master
       if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: Pairing Duration = %lu (us)\n", true, false, micros() - time);}

       if (!isPaired) setAlarm(false, 10);  // RTC alarms in 10 second's time
      }
      else
      {
       radioHandshake();  // Attempt to Pair with Master
      }

      if (isPaired) setAlarm(false);  // If Paired successfully, set device to update in CAP_CHARGE_DELAY_MINUTES time and then every hour thenceforth, (because the next matching minutes and seconds will then be an hour away)
      else if (pairingCounter >= NUM_PAIRING_ATTEMPTS)
      {
       pinStr(POWER_OFF_PIN, HIGH);     // Set the pin to High Drive Strength
       pinMode(POWER_OFF_PIN, OUTPUT);  // Kill the Power if the device hasn't paired after NUM_PAIRING_ATTEMPTS
       digitalWrite(POWER_OFF_PIN, LOW);
      }
     }
     else
     {
      unsigned long time;
      if (ENABLE_VERBOSE) {time = micros();}
      if (radioHandshake())  // Send Data
      {
       if (ENABLE_VERBOSE) {serialPrintf(USE_SERIAL, serialbuf, "CLIENT: Send Data Duration = %lu (us)\n", true, false, micros() - time);}
//       setAlarm(false, 3); // Send next data in 3 seconds time, (This is for accelerated data testing)
      }
      else setAlarm(true);  // If sending data failed, try again in CAP_CHARGE_DELAY_MINUTES + some random seconds, (< 60)
     }

     radio.sleep();      // Put Radio back to sleep
    }

    if (USE_SERIAL) USBDevice.detach();

    rtc.standbyMode();   // Sleep until next alarm match
   #endif
  }
 #else
  if (radioInitialised) radioHandshake();  // If Paired, send Valid Data else send a Pairing Request
  delay(LOOP_DELAY);
 #endif
}
