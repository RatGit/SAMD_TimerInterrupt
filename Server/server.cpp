////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//  PROJECT:      Wireless Sensor Controller                                                                          //
//                                                                                                                    //
//  MODULE:       server.cpp                                                                                          //
//                                                                                                                    //
//  DESCRIPTION:  LoRa Packet Radio Server. Used for testing Power Utilisation, Comms Contention etc.                 //
//                                                                                                                    //
//  AUTHORS:      Keith Willis (keith.willis@vutronix.com)                                                            //
//                                                                                                                    //
//  NOTES:        Hardware Requirements: Mini Ultra Pro board powered by a single cell Li-Ion/Pol 3.7V battery.       //
//                                                                                                                    //
//                Creates a reliable packet radio server with support for pairing, addressing and automated retries.  //
//                                                                                                                    //
//                It is designed to work with the "link-test-client" sketch                                           //
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
//   Notes:        It replaces the 24AA02E64 OUI with a custom one, (SERVER_OUI).                                     //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     initClients                                                                                        //
//   Description:  Clear list of client UID's                                                                         //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    none                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     numClients                                                                                         //
//   Description:  Count the number of assigned clients                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns the number of assigned clients on success                                                  //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     findClient                                                                                         //
//   Description:  Search list of client UID's for client UID                                                         //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns client address on success                                                                  //
//                 Returns NUM_CLIENTS on fail                                                                        //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     nextClient                                                                                         //
//   Description:  Find next available client address                                                                 //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns next available client address on success                                                   //
//                 Returns NUM_CLIENTS on fail                                                                        //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     addClient                                                                                          //
//   Description:  Assign a client address and add to list of client UID's                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns new client address on success                                                              //
//                 Returns NUM_CLIENTS on fail                                                                        //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     removeClient                                                                                       //
//   Description:  Remove a client address from the list of client UID's                                              //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      bool                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
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
//   Function:     radioRead                                                                                          //
//   Description:  This function is called once every program loop. It waits for a message from a client and then     //
//                 sends a reply message, echoing both out of the serial port. It also toggles the LED on a           //
//                 successful handshake.                                                                              //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Called in "loop"                                                                                   //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     convertStrToUint                                                                                   //
//   Description:  Convert a Str to a 32-Bit Unsigned Integer.                                                        //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* str                                                                                          //
//   Returns:      uint32_t                                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     serialRead                                                                                         //
//   Description:  Read serial data and assemble into a data packet.                                                  //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        If serial data overflows message buffer, the buffer pointer is reset to start                      //
//                 Packets have an initial command character, followed by optional data and terminated with a         //
//                 Line Feed character                                                                                //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     setup                                                                                              //
//   Description:  Main sketch initialisation function. Sets unused IO pins to their lowest power mode, (ie. Input    //
//                 + PullUp), puts the Serial Flash to sleep, configures the LoRa radio module and initialises the    //
//                 RTC module.                                                                                        //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:                                                                                                           //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     loop                                                                                               //
//   Description:  Main sketch program loop. Calls the "radioRead" function once every program loop.                  //
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
#include "server.h"


//////////////////////////////////////
//  Global Variables and Constants  //
//////////////////////////////////////

RHReliableDatagram manager(radio, DEFAULT_SERVER_ADDRESS);  // Class to manage message delivery and receipt, using the driver declared above

uint64_t clientList[NUM_CLIENTS];                           // List of paired client UID's
char CLIENT_LIST_FILENAME [] = "CLIENT_LIST_FILENAME";      // Serial Flash File Name to store list of Clients
bool serialFlashOk;                                         // Boolean Flag to indicate if Serial Flash was properly initialised
bool clientListFileExists;                                  // Boolean Flag to indicate if Serial Flash Client List File was successfully created

char serialData[SERIAL_PACKET_LENGTH];
bool serialReceived;
uint8_t serialPtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     getUID                                                                                             //
//   Description:  This function returns a 64-bit UID read from the 24AA02E64 IC                                      //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* _uidstr                                                                                      //
//   Returns:      uint64_t                                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        It replaces the 24AA02E64 OUI with a custom one, (SERVER_OUI).                                     //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t getUID(char* _uidstr)
{
 Wire.beginTransmission(EUI64_CHIP_ADDRESS);
 Wire.write(UID_ADDRESS);
 Wire.endTransmission();
 Wire.requestFrom(EUI64_CHIP_ADDRESS, UID_LENGTH);

 unsigned char uidbuf[UID_LENGTH];

 uint8_t ptr = 0;
 while (Wire.available()) uidbuf[ptr++] = Wire.read(); // Format needs to be little endian (LSB...MSB)

 sprintf(_uidstr, "%s%02X%02X%02X%02X%02X", SERVER_OUI, uidbuf[0], uidbuf[1], uidbuf[2], uidbuf[3], uidbuf[4]);

 return strtoull(_uidstr, NULL, 16);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     id2chip                                                                                            //
//   Description:                                                                                                     //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    none                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char * id2chip(const unsigned char *id)
{
  if (id[0] == 0xEF) {
    // Winbond
    if (id[1] == 0x40) {
      if (id[2] == 0x14) return "W25Q80BV";
      if (id[2] == 0x17) return "W25Q64FV";
      if (id[2] == 0x18) return "W25Q128FV";
      if (id[2] == 0x19) return "W25Q256FV";
    }
  }
  if (id[0] == 0x01) {
    // Spansion
    if (id[1] == 0x02) {
      if (id[2] == 0x16) return "S25FL064A";
      if (id[2] == 0x19) return "S25FL256S";
      if (id[2] == 0x20) return "S25FL512S";
    }
    if (id[1] == 0x20) {
      if (id[2] == 0x18) return "S25FL127S";
    }
  }
  if (id[0] == 0xC2) {
    // Macronix
    if (id[1] == 0x20) {
      if (id[2] == 0x18) return "MX25L12805D";
    }
  }
  if (id[0] == 0x20) {
    // Micron
    if (id[1] == 0xBA) {
      if (id[2] == 0x20) return "N25Q512A";
      if (id[2] == 0x21) return "N25Q00AA";
    }
    if (id[1] == 0xBB) {
      if (id[2] == 0x22) return "MT25QL02GC";
    }
  }
  if (id[0] == 0xBF) {
    // SST
    if (id[1] == 0x25) {
      if (id[2] == 0x02) return "SST25WF010";
      if (id[2] == 0x03) return "SST25WF020";
      if (id[2] == 0x04) return "SST25WF040";
      if (id[2] == 0x41) return "SST25VF016B";
      if (id[2] == 0x4A) return "SST25VF032";
    }
    if (id[1] == 0x25) {
      if (id[2] == 0x01) return "SST26VF016";
      if (id[2] == 0x02) return "SST26VF032";
      if (id[2] == 0x43) return "SST26VF064";
    }
  }
  return "(unknown chip)";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     initClients                                                                                        //
//   Description:  Clear list of client UID's                                                                         //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    none                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initClients()
{
 if (serialFlashOk)
 {
  clientListFileExists = SerialFlash.exists(CLIENT_LIST_FILENAME);
  if (clientListFileExists)
  {
   SerialFlashFile clientListFile;
   clientListFile = SerialFlash.open(CLIENT_LIST_FILENAME);
   if ((bool)clientListFile)
   {
    char clientListBuffer[NUM_CLIENTS * 64];

    clientListFile.read(clientListBuffer, NUM_CLIENTS * 64);
    clientListFile.close();

    for (uint8_t i=0; i<NUM_CLIENTS; i++) clientList[i] = ((uint64_t*)clientListBuffer)[i];
   }
   else
   {
    for (uint8_t i=0; i<NUM_CLIENTS; i++) clientList[i] = 0;
   }
  }
  else
  {
   clientListFileExists = SerialFlash.createErasable(CLIENT_LIST_FILENAME, NUM_CLIENTS * 64);

   if (clientListFileExists)
   {
    SerialFlashFile clientListFile;
    clientListFile = SerialFlash.open(CLIENT_LIST_FILENAME);
    if ((bool)clientListFile)
    {
     char clientListBuffer[NUM_CLIENTS * 64];

     for (uint8_t i=0; i<NUM_CLIENTS; i++) ((uint64_t*)clientListBuffer)[i] = 0;

     clientListFile.seek(0);
     clientListFile.write(clientListBuffer, NUM_CLIENTS * 64);
     clientListFile.close();
    }
   }

   for (uint8_t i=0; i<NUM_CLIENTS; i++) clientList[i] = 0;
  }

//  SerialFlash.sleep();
 }
 else
 {
  clientListFileExists = false;
  for (uint8_t i=0; i<NUM_CLIENTS; i++) clientList[i] = 0;
 }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     numClients                                                                                         //
//   Description:  Count the number of assigned clients                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns the number of assigned clients on success                                                  //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t numClients()
{
 uint8_t i, clientCount = 0;

 for (i=0; i<NUM_CLIENTS; i++)
 {
  if (clientList[i] != 0) clientCount++;
 }

 return clientCount;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     findClient                                                                                         //
//   Description:  Search list of client UID's for client UID                                                         //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      uint8_t                                                                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns client address on success                                                                  //
//                 Returns NUM_CLIENTS on fail                                                                        //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t findClient(uint64_t clientUID)
{
 uint8_t i;

 for (i=0; i<NUM_CLIENTS; i++)
 {
  if (clientList[i] == clientUID) return i;
 }

 return i;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     addClient                                                                                          //
//   Description:  Assign a client address and add to list of client UID's                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      uint8_t                                                                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns new client address on success                                                              //
//                 Returns NUM_CLIENTS on fail                                                                        //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t addClient(uint64_t clientUID)
{
 uint8_t i = findClient(clientUID);
 if (i < NUM_CLIENTS) return i;

 for (i=0; i<NUM_CLIENTS; i++)
 {
  if (clientList[i] == 0)
  {
   if (clientListFileExists)
   {
//    SerialFlash.wakeup();
//    while (SerialFlash.ready() == false) delay(100);

    SerialFlashFile clientListFile;
    clientListFile = SerialFlash.open(CLIENT_LIST_FILENAME);
    if ((bool)clientListFile)
    {
     char clientListBuffer[NUM_CLIENTS * 64];

     clientListFile.read(clientListBuffer, NUM_CLIENTS * 64);

     ((uint64_t*)clientListBuffer)[i] = clientUID;

     clientListFile.erase();
     clientListFile.seek(0);
     clientListFile.write(clientListBuffer, NUM_CLIENTS * 64);

     clientListFile.close();
    }

//    SerialFlash.sleep();
   }

   clientList[i] = clientUID;
   return i;
  }
 }

 return i;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     nextClient                                                                                         //
//   Description:  Find next available client address                                                                 //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns next available client address on success                                                   //
//                 Returns NUM_CLIENTS on fail                                                                        //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t nextClient(uint64_t clientUID)
{
 uint8_t i;

 for (i=0; i<NUM_CLIENTS; i++)
 {
  if (clientList[i] == 0) return i;
 }

 return i;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     removeClient                                                                                       //
//   Description:  Remove a client address from the list of client UID's                                              //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    uint64_t clientUID                                                                                 //
//   Returns:      bool                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool removeClient(uint64_t clientUID)
{
 for (uint8_t i=0; i<NUM_CLIENTS; i++)
 {
  if (clientList[i] == clientUID)
  {
   clientList[i] = 0;
   return true;
  }
 }

 return false;
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
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     radioRead                                                                                          //
//   Description:  This function is called once every program loop. It waits for a message from a client and then     //
//                 sends a reply message, echoing both out of the serial port. It also toggles the LED on a           //
//                 successful handshake.                                                                              //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Called in "loop"                                                                                   //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool radioRead()
{
 char buf[255];

 uint8_t len=RH_RF95_MAX_MESSAGE_LEN, from;

 if (manager.recvfromAck(msgBuffer, &len, &from))
 {
  digitalWrite(LED_BUILTIN, HIGH);
  msgBuffer[len] = 0;
  if (VERBOSE) serialPrintf(buf, "SERVER: [%u < %u|%d]: \"%s\"[%d]", true, manager.headerTo(), from, numClients(), (char*)msgBuffer, len);
//  if (VERBOSE) serialPrintf(buf, "Message Received: \"%s\" Length=%d To=%d From=%d : Number of Clients=%d", true, (char*)msgBuffer, len, manager.headerTo(), from, numClients());
  else serialPrintf(buf, "%03d|%03d: \"%s\"", true, from, numClients(), (char*)msgBuffer);

  if (manager.headerTo() == PAIRING_ADDRESS  && len == 16)  // This is a Pairing Request (with a 16byte message = 24bit OUI + 40bit UID)  Pairing Packet: <OUI><UID>  eg: 0004A30B001A534A
  {
   if (PAIRING_ENABLED)
   {
    uint64_t clientUID = strtoull((char*)msgBuffer, NULL, 16);  // Convert 16 hex chars into a 64bit OUI+UID

    uint8_t tempChar = msgBuffer[6];
    msgBuffer[6] = 0;
    uint32_t clientOUI = strtoul((char*)msgBuffer, NULL, 16);  // Convert first 6 hex chars int a 24bit OUI
    if (VERBOSE && clientOUI != serverOUI) {serialPrint((char*)"Ignoring Pairing Request, (different OUI)\n"); return false;}  // Ignore pairing requests from clients that don't belong to us, (ie. different OUI)
    msgBuffer[6] = tempChar;

    uint8_t clientAddress = findClient(clientUID);
    if (clientAddress < NUM_CLIENTS) {if (VERBOSE) serialPrint((char *)"Client already paired");}
    else clientAddress = nextClient(clientUID);

    if (clientAddress == NUM_CLIENTS) {if (VERBOSE) serialPrint((char *)"Client pairing failed, no addresses available");}
    else
    {
     if (VERBOSE) {serialPrintf(buf, "Client Pairing Request received: New client address = %d", true, clientAddress+1);}

     uint8_t data[34];
     sprintf((char*)data, "%s:%02X:%02X", msgBuffer, DEFAULT_SERVER_ADDRESS, clientAddress+1);  // Create the Pairing Request Response datagram: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS> From SERVER_ADDRESS To DEFAULT_CLIENT_ADDRESS

     if (VERBOSE) {serialPrintf(buf, "Sending Pairing Request Response: \"<Client OUI:UID>:<Server Address>:<Client Address>\" = \"%s\"", true, (char*)data);}

     if (manager.sendtoWait(data, 22, from))  // Send the Pairing Request Response: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS> From SERVER_ADDRESS To DEFAULT_CLIENT_ADDRESS
     {
      #ifndef LOW_POWER
       serialPrint((char *)"Waiting for Pairing Request Response Handshake");
      #endif

      uint64_t clientUID2;
      uint8_t clientAddress2, retries=NUM_RETRIES;
      bool PairingRequestResponseHandshakeReceived = false;
      while (!PairingRequestResponseHandshakeReceived && retries-- > 0)
      {
       len = 19;
       if (manager.recvfromAckTimeout(msgBuffer, &len, CLIENT_ACK_TIMEOUT, &from))  // Now wait for a Pairing Request Response from the server: <8bit SERVER ADDRESS><8bit CLIENT ADDRESS>
       {
        msgBuffer[19] = 0;

        if (VERBOSE) serialPrintf(buf, "SERVER: [%u|%d < %u]: \"%s\"[%d]", true, manager.headerTo(), numClients(), from, (char*)msgBuffer, len);
//        if (VERBOSE) serialPrintf(buf, "Message Received: \"%s\" Length=%d To=%d From=%d : Number of Clients=%d", true, (char*)msgBuffer, len, manager.headerTo(), from, numClients());

        if (manager.headerTo() == PAIRING_ADDRESS && len == 19)  // This is a Pairing Request Response Handshake: <64bit OUI:UID>:<8bit CLIENT ADDRESS> From CLIENT_ADDRESS To PAIRING_ADDRESS
        {
         uint8_t clientAddress2 = strtoul((char*)(&(msgBuffer[17])), NULL, 16);  // Convert last 2 hex chars into an 8bit Client Address
         msgBuffer[16] = 0;
         clientUID2 = strtoull((char*)msgBuffer, NULL, 16);                      // Convert first 16 hex chars into a 64bit OUI:UID
         PairingRequestResponseHandshakeReceived = (clientUID2 == clientUID && clientAddress2 == clientAddress+1);  // This packet's client OUI:UID = previous packet's as does the client addresses
        }
       }
      }

      if (PairingRequestResponseHandshakeReceived)
      {
       digitalWrite(LED_BUILTIN, HIGH);
       msgBuffer[len] = 0;
       if (VERBOSE)
       {
        msgBuffer[16] = ':';
        serialPrintf(buf, "Valid Pairing Request Response Handshake Received: \"%s\"", true, (char*)msgBuffer);
       }

       msgBuffer[16] = 0;

       sprintf((char*)data, "%s:%02X:%02X:", msgBuffer, DEFAULT_SERVER_ADDRESS, clientAddress+1);  // Create the Pairing Response Handshake Acknowledgement datagram: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS>:<Timestamp> From SERVER_ADDRESS To CLIENT_ADDRESS
       getTimestampStr((char*)(&(data[23])));  // Append 10 characters containing the 100 year decimal timestamp (in seconds)

       #ifndef LOW_POWER
        serialPrintf(buf, "Sending Pairing Request Response Handshake Acknowledgement: \"%s\" To: %d", true, (char*)data, from);
       #endif

       if (manager.sendtoWait(data, 33, from))
       {
        #ifndef LOW_POWER
         serialPrint((char *)"Pairing Request Response Handshake Acknowledgement Sent");
        #endif

        clientAddress = addClient(clientUID);
        if (clientAddress == NUM_CLIENTS) {if (VERBOSE) serialPrint((char *)"Client pairing failed, no addresses available");}
        else
        {
         if (VERBOSE) serialPrint((char *)"Client successfully paired");
         digitalWrite(LED_BUILTIN, LOW);
        }
       }
       else if (VERBOSE) serialPrint((char *)"Pairing client failed to ACK Pairing Request Response Handshake Acknowledgement");
      }
      else if (VERBOSE) serialPrint((char *)"Pairing client failed to Handshake Pairing Request Response");
     }
     else if (VERBOSE) serialPrint((char *)"Pairing client failed to ACK Pairing Request Response");
    }
   }
   else if (VERBOSE) serialPrint((char *)"Pairing disabled");
  }
  else if (len == PACKET_LENGTH) // Data Packet: <OUI><UID>:<TEMP>:<RH>:<CRC>  eg: "0004A30B001A531C:DF7:234:7E"
  {
   uint8_t calc_crc = crc((void*)msgBuffer, PACKET_LENGTH-2);                             // Calculate message CRC
   uint8_t msg_crc = (uint8_t)strtoul((char*)(&(msgBuffer[PACKET_LENGTH-2])), NULL, 16);  // Convert last 2 hex chars into an 8bit CRC
   if (calc_crc != msg_crc)                                                               // Ignore messages with invalid CRC
   {
    if (VERBOSE) serialPrint((char *)"Invalid Message CRC\n");
    return false;
   }

   uint8_t swapChar = msgBuffer[6];
   msgBuffer[6] = 0;
   uint32_t clientOUI = strtoul((char*)msgBuffer, NULL, 16);  // Convert first 6 hex chars into a 24bit OUI
   if (clientOUI != serverOUI)                                // Ignore messages from clients that don't belong to us, (ie. different OUI)
   {
    if (VERBOSE) serialPrint((char *)"Client does not belong to this organisation\n");
    return false;
   }
   msgBuffer[6] = swapChar;

   swapChar = msgBuffer[16];
   msgBuffer[16] = 0;
   uint64_t clientUID = strtoull((char*)msgBuffer, NULL, 16);  // Convert first 16 hex chars into a 64bit OUI+UID
   if (findClient(clientUID) == NUM_CLIENTS)                   // Ignore messages from clients not managed by this gateway
   {
    if (VERBOSE) serialPrint((char *)"Client not managed by this gateway\n");
    return false;
   }
   msgBuffer[16] = swapChar;

//    uint8_t data[2];
//    sprintf((char*)data, "%02X", DEFAULT_SERVER_ADDRESS);  // Acknowledge the client's data packet with this server's address
//
//    if (manager.sendtoWait(data, sizeof(data), from))
//    {
    digitalWrite(LED_BUILTIN, LOW);
//
//    if (VERBOSE) {serialPrint(buf, "Reply:   \"%s\"", true, (char*)data);}
//    }
//    else if (VERBOSE) serialPrint((char*)"Reply Failed");
  }
  else if (VERBOSE) serialPrint((char *)"Invalid Packet");

  if (VERBOSE) serialPrint((char *)"");
 }

 return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     convertStrToUint                                                                                   //
//   Description:  Convert a Str to a 32-Bit Unsigned Integer.                                                        //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* str                                                                                          //
//   Returns:      uint32_t                                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t convertStrToUint(char* str)
{
 uint32_t result = 0, exponent = 1;
 int pow = strlen(str);

 for (uint8_t i=0; i<pow-1; i++)
 {
  result += ((str[pow-i-1] - 48) * exponent);
  exponent *= 10;
 }

 return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     serialRead                                                                                         //
//   Description:  Read serial data and assemble into a data packet.                                                  //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        If serial data overflows message buffer, the buffer pointer is reset to start                      //
//                 Packets have an initial command character, followed by optional data and terminated with a         //
//                 Line Feed character                                                                                //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void serialRead()
{
 while (Serial.available() > 0)
 {
  serialData[serialPtr] = Serial.read();
  if (serialData[serialPtr] == SERIAL_END_CHAR) serialReceived = true;
  else {if (++serialPtr >= SERIAL_PACKET_LENGTH) serialPtr = 0;}
 }

 if (serialReceived)
 {
  serialData[serialPtr] = 0;

  if (serialData[0] == 'T')  // Timestamp <T0123456789>
  {
//   uint32_t timestamp = strtoul(&(serialData[1]), NULL, 10);  // Convert remaining chars into a 32-bit timestamp
   uint32_t timestamp = convertStrToUint(&(serialData[1]));  // Convert remaining chars into a 32-bit timestamp
   rtc.setY2kEpoch(timestamp);

   char pbuf[255];
////   serialPrint((char*)(&(serialData[1])));
//   serialPrintf(pbuf, "Timestamp = %u", (uint32_t)(strtoul(&(serialData[1]), NULL, 10)));
//   serialPrintf(pbuf, "Timestamp = %u", convertStrToUint(serialData));

   getDateTime();
//   serialPrintf(pbuf, "rtc.getY2kEpoch() = %u", rtc.getY2kEpoch());
//   serialPrintf(pbuf, "rtc.getDay() = %u", dateTime.day);
//   serialPrintf(pbuf, "rtc.getMonth() = %u", dateTime.month);
//   serialPrintf(pbuf, "rtc.getYear() = %u", dateTime.year);
//   serialPrintf(pbuf, "rtc.getHours() = %u", dateTime.hour);
//   serialPrintf(pbuf, "rtc.getMinutes() = %u", dateTime.minute);
//   serialPrintf(pbuf, "rtc.getSeconds() = %u", dateTime.second);
  }

  serialPtr = 0;
  serialReceived = false;
 }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     setup                                                                                              //
//   Description:  Main sketch initialisation function. Sets unused IO pins to their lowest power mode, (ie. Input    //
//                 + PullUp), puts the Serial Flash to sleep, configures the LoRa radio module and initialises the    //
//                 RTC module.                                                                                        //
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

 serverOUI = strtoul(SERVER_OUI, NULL, 16);

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

 // Initialise LED
 pinMode(LED_BUILTIN, OUTPUT);
 #ifdef LOW_POWER
  digitalWrite(LED_BUILTIN, LOW);   // Turn Off LED
 #else
  digitalWrite(LED_BUILTIN, HIGH);  // Turn On LED
 #endif

 // USB Serial Port Initialisation
 #ifdef USE_SERIAL
  Serial.begin(BAUD_RATE);
  while (WAIT_SERIAL && !Serial) ; // Wait for serial port to be available (This will cause it to hang when not connected to a USB serial port)
  #ifdef MONITOR_SERIAL
   delay(15000);  // Wait 15 seconds to allow enough time to connect a serial monitor
  #endif
 #endif

 // RTC initialisation
 rtc.begin();
 rtc.setTime(dateTime.hour, dateTime.minute, dateTime.second);
 rtc.setDate(dateTime.day, dateTime.month, dateTime.year);

 // Serial Flash Initialisation
 serialFlashOk = SerialFlash.begin(flashChipSelect);
 #if !defined(LOW_POWER) && defined(VERBOSE)
  if (!serialFlashOk) serialPrint("Failed to Initialise Serial Flash", true);
 #endif

 // I2C Initialisation
 Wire.begin();

 // LoRa Radio Initialisation
 radioInitialised = manager.init();
 if (!radioInitialised)
 {
  #if !defined(LOW_POWER) && defined(VERBOSE)
   serialPrint("Failed to Initialise LoRa Radio");
  #endif
 }
 else
 {
  radio.setFrequency(RADIO_FREQ);
  radio.setTxPower(RADIO_POWER, false);
//  radio.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
//  manager.setRetries(5);
 }

 uid = getUID(uidstr);  // Read the 64-bit UID read from the 24AA02E64 IC

 initClients();

 serialPtr = 0;
 serialReceived = false;

 #ifndef LOW_POWER
  digitalWrite(LED_BUILTIN, LOW);
 #endif

 #if !defined(LOW_POWER) && defined(VERBOSE)
  char buff[255];
  uint32_t low = uid & 0xFFFFFFFF, high = (uid >> 32) & 0xFFFFFFFF;
  serialPrintf(buff, "Starting Server: UID = 0x%08X%08X\n", true, high, low);
 #endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     loop                                                                                               //
//   Description:  Main sketch program loop. Calls the "radioRead" and "serialRead" functions once every program loop //
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
 radioRead();
 serialRead();
}
