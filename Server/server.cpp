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
//                **Important**: When using the "SPIFlash.powerDown()" function, the only function that the serial    //
//                flash chip will then respond to is "SPIFlash.powerUp()".                                            //
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
//   Arguments:    None                                                                                               //
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
//   Arguments:    None                                                                                               //
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
//   Returns:      bool, (true if Successful, false if Fail)                                                          //
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
//   Returns:      bool, (true if Successful, false if Fail)                                                          //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        If serial data overflows message buffer, the buffer pointer is reset to start                      //
//                 Packets have an initial command character, followed by optional data, an 8-bit CRC and             //
//                 terminated with a  Line Feed character                                                             //
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


SPIFlash SerialFlash(flashChipSelect); //, &SPI);
RHReliableDatagram manager(radio, DEFAULT_SERVER_ADDRESS);  // Class to manage message delivery and receipt, using the driver declared above

uint64_t clientList[NUM_CLIENTS];                           // List of paired client UID's
char CLIENT_LIST_FILENAME [] = "CLIENT_LIST_FILENAME";      // Serial Flash File Name to store list of Clients
bool pairingEnabled = false;                                // Enable Pairing
bool serialFlashOk;                                         // Boolean Flag to indicate if Serial Flash was properly initialised
bool clientListFileExists;                                  // Boolean Flag to indicate if Serial Flash Client List File was successfully created

char serialData[SERIAL_PACKET_LENGTH];
bool serialReceived;
uint8_t serialPtr;


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
  uint32_t clientListAddress = 0; //SerialFlash.getAddress(sizeof(clientList)/sizeof(clientList[0]));
  if (ENABLE_VERBOSE) {serialPrintf(serialbuf, "SERVER: Client List Address = 0x%08lX", true, false, clientListAddress);}

  if (ENABLE_VERBOSE)
  {
   for (uint8_t i=0; i<10; i++)
   {
    uint32_t low = clientList[i] & 0xFFFFFFFF, high = (clientList[i] >> 32) & 0xFFFFFFFF;
    serialPrintf(serialbuf, "SERVER: Client List[%u] = 0x%08lX%08lX", true, false, i, high, low);
   }
  }

  for (uint8_t i=0; i<10; i++) clientList[i] = (uint64_t)i;

  if (ENABLE_VERBOSE)
  {
   for (uint8_t i=0; i<10; i++)
   {
    uint32_t low = clientList[i] & 0xFFFFFFFF, high = (clientList[i] >> 32) & 0xFFFFFFFF;
    serialPrintf(serialbuf, "SERVER: Client List[%u] = 0x%08lX%08lX", true, false, i, high, low);
   }
  }

  bool writeOk = SerialFlash.writeByteArray(clientListAddress, (uint8_t*)clientList, NUM_CLIENTS*sizeof(clientList[0]), true);
  if (ENABLE_VERBOSE)
  {
   if (writeOk) serialPrintf(serialbuf, "SERVER: Successfully wrote Client List to Flash at Address = 0x%08lX", true, false, clientListAddress);
   else serialPrintf(serialbuf, "SERVER: Failed to write Client List to Flash at Address = 0x%08lX", true, false, clientListAddress);
  }

  if (writeOk)
  {
   bool readOk = SerialFlash.readByteArray(clientListAddress, (uint8_t*)clientList, NUM_CLIENTS*sizeof(clientList[0]));
   if (ENABLE_VERBOSE)
   {
    if (readOk) serialPrintf(serialbuf, "SERVER: Successfully read Client List from Flash at Address = 0x%08lX", true, false, clientListAddress);
    else serialPrintf(serialbuf, "SERVER: Failed to read Client List from Flash at Address = 0x%08lX", true, false, clientListAddress);
   }
  }

  if (ENABLE_VERBOSE)
  {
   for (uint8_t i=0; i<10; i++)
   {
    uint32_t low = clientList[i] & 0xFFFFFFFF, high = (clientList[i] >> 32) & 0xFFFFFFFF;
    serialPrintf(serialbuf, "SERVER: Client List[%u] = 0x%08lX%08lX", true, false, i, high, low);
   }
  }
/*
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
*/
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
/*
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
*/
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
//   Arguments:    None                                                                                               //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Returns next available client address on success                                                   //
//                 Returns NUM_CLIENTS on fail                                                                        //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t nextClient()
{
 uint8_t i;

 for (i=0; i<NUM_CLIENTS; i++)
 {
  if (clientList[i] == 0) return i;
 }

 return i;  // i == NUM_CLIENTS on fail
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
//   Returns:      bool, (true if Successful, false if Fail)                                                          //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Called in "loop"                                                                                   //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool radioRead()
{
 uint8_t len=RH_RF95_MAX_MESSAGE_LEN, from;

 if (manager.recvfromAck(msgBuffer, &len, &from))  // If there is a valid message available for this node, send an acknowledgement to the SRC address (blocking until this is complete), then copy the message to serialbuf and return true else return false
 {
  digitalWrite(LED_BUILTIN, HIGH);  // Turn ON LED

  msgBuffer[len] = 0;
  if (ENABLE_VERBOSE) serialPrintf(serialbuf, "SERVER: [%u < %u|%d]: \"%s\"[%d]", true, false, manager.headerTo(), from, numClients(), (char*)msgBuffer, len);
//  if (ENABLE_VERBOSE) serialPrintf(serialbuf, "Message Received: \"%s\" Length=%d To=%d From=%d : Number of Clients=%d", true, false, (char*)msgBuffer, len, manager.headerTo(), from, numClients());
//  else serialPrintf(serialbuf, "%03d|%03d: \"%s\"", true, false, from, numClients(), (char*)msgBuffer);

  if (manager.headerTo() == PAIRING_ADDRESS  && len == 16)  // This is a Pairing Request (with a 16byte message = 24bit OUI + 40bit UID)  Pairing Packet: <OUI><UID>  eg: 0004A30B001A534A
  {
   if (!pairingEnabled) {if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Pairing disabled");}

   uint64_t clientUID = strtoull((char*)msgBuffer, NULL, 16);  // Convert 16 hex chars into a 64bit OUI+UID

   uint8_t tempChar = msgBuffer[6];
   msgBuffer[6] = 0;
   uint32_t clientOUI = strtoul((char*)msgBuffer, NULL, 16);  // Convert first 6 hex chars int a 24bit OUI
   msgBuffer[6] = tempChar;

   if (clientOUI != serverOUI)
   {
    if (ENABLE_VERBOSE) {serialPrint((char*)"SERVER: Ignoring Pairing Request, (different OUI)\n");}  // Ignore pairing requests from clients that don't belong to us, (ie. different OUI)
    digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED
    return false;
   }

   uint8_t clientAddress = findClient(clientUID);
   if (clientAddress < NUM_CLIENTS) {if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Client already paired");}
   else clientAddress = nextClient();
   if (clientAddress == NUM_CLIENTS) {if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Client pairing failed, no addresses available");}

   if (!pairingEnabled || clientAddress == NUM_CLIENTS)  // Handle case for Pairing Disabled or no Client addresses available
   {
    uint8_t data[34];
    sprintf((char*)data, "%s:%02X:%02X", msgBuffer, DEFAULT_SERVER_ADDRESS, 255);  // Create the Pairing Request Response datagram: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit INVALID CLIENT ADDRESS> From SERVER_ADDRESS To DEFAULT_CLIENT_ADDRESS

    if (ENABLE_VERBOSE) {serialPrintf(serialbuf, "SERVER: Sending Invalid Pairing Request Response: \"<Client OUI:UID>:<Server Address>:<Client Address>\" = \"%s\"", true, false, (char*)data);}

    if (manager.sendtoWait(data, 22, from))  // Send an Invalid Pairing Request Response: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit INVALID CLIENT ADDRESS> From SERVER_ADDRESS To DEFAULT_CLIENT_ADDRESS
    {
     if (ENABLE_VERBOSE) {serialPrintf(serialbuf, "SERVER: Sent Invalid Pairing Request Response: \"<Client OUI:UID>:<Server Address>:<Client Address>\" = \"%s\"", true, false, (char*)data);}
    }

    digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED
    return false;
   }
   else
   {
    if (ENABLE_VERBOSE) {serialPrintf(serialbuf, "SERVER: Client Pairing Request received: New client address = %d", true, false, clientAddress+1);}

    uint8_t data[34];
    sprintf((char*)data, "%s:%02X:%02X", msgBuffer, DEFAULT_SERVER_ADDRESS, clientAddress+1);  // Create the Pairing Request Response datagram: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS> From SERVER_ADDRESS To DEFAULT_CLIENT_ADDRESS

    if (ENABLE_VERBOSE) {serialPrintf(serialbuf, "SERVER: Sending Pairing Request Response: \"<Client OUI:UID>:<Server Address>:<Client Address>\" = \"%s\"", true, false, (char*)data);}

    if (manager.sendtoWait(data, 22, from))  // Send the Pairing Request Response: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS> From SERVER_ADDRESS To DEFAULT_CLIENT_ADDRESS
    {
     if (ENABLE_VERBOSE) {serialPrint((char *)"SERVER: Waiting for Pairing Request Response Handshake");}

     uint64_t clientUID2;
     uint8_t retries = NUM_RETRIES;
     bool PairingRequestResponseHandshakeReceived = false;
     while (!PairingRequestResponseHandshakeReceived && retries-- > 0)
     {
      len = 19;
      if (manager.recvfromAckTimeout(msgBuffer, &len, CLIENT_ACK_TIMEOUT, &from))  // Now wait for a Pairing Request Response from the server: <8bit SERVER ADDRESS><8bit CLIENT ADDRESS>
      {
       msgBuffer[19] = 0;

       if (ENABLE_VERBOSE) serialPrintf(serialbuf, "SERVER: [%u|%d < %u]: \"%s\"[%d]", true, false, manager.headerTo(), numClients(), from, (char*)msgBuffer, len);
//       if (ENABLE_VERBOSE) serialPrintf(serialbuf, "Message Received: \"%s\" Length=%d To=%d From=%d : Number of Clients=%d", true, false, (char*)msgBuffer, len, manager.headerTo(), from, numClients());

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
      if (ENABLE_VERBOSE)
      {
       msgBuffer[16] = ':';
       serialPrintf(serialbuf, "SERVER: Valid Pairing Request Response Handshake Received: \"%s\"", true, false, (char*)msgBuffer);
      }

      msgBuffer[16] = 0;

      sprintf((char*)data, "%s:%02X:%02X:", msgBuffer, DEFAULT_SERVER_ADDRESS, clientAddress+1);  // Create the Pairing Response Handshake Acknowledgment datagram: <64bit OUI:UID>:<8bit SERVER ADDRESS>:<8bit CLIENT ADDRESS>:<Timestamp> From SERVER_ADDRESS To CLIENT_ADDRESS
      getTimestampStr((char*)(&(data[23])));  // Append 10 characters containing the 100 year decimal timestamp (in seconds)

      if (ENABLE_VERBOSE) {serialPrintf(serialbuf, "SERVER: Sending Pairing Request Response Handshake Acknowledgment: \"%s\" To: %d", true, false, (char*)data, from);}

      if (manager.sendtoWait(data, 33, from))
      {
       if (ENABLE_VERBOSE) {serialPrint((char *)"SERVER: Pairing Request Response Handshake Acknowledgment Sent");}

       clientAddress = addClient(clientUID);
       if (clientAddress == NUM_CLIENTS) {if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Client pairing failed, no addresses available");}
       else
       {
        data[16] = 0;
//        serialPrint(ulltohex(serialbuf, clientUID));
        if (ENABLE_VERBOSE) serialPrintf(serialbuf, "%s [%s]", true, false, "SERVER: Client successfully paired", data);
        else {serialPrintf(serialbuf, "%s:%s:", true, true, CLIENT_PAIRED, data);}
       }
      }
      else if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Pairing client failed to ACK Pairing Request Response Handshake Acknowledgment");
     }
     else if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Pairing client failed to Handshake Pairing Request Response");
    }
    else if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Pairing client failed to ACK Pairing Request Response");
   }
  }
  else if (len == PACKET_LENGTH) // Data Packet: <OUI><UID>:<TEMP>:<RH>:<CRC>  eg. "0004A30B001A531C:123.45:123.45:7E"
  {
   uint8_t calc_crc = crc((void*)msgBuffer, PACKET_LENGTH-2);                             // Calculate message CRC
   uint8_t msg_crc = (uint8_t)strtoul((char*)(&(msgBuffer[PACKET_LENGTH-2])), NULL, 16);  // Convert last 2 hex chars into an 8bit CRC
   if (calc_crc != msg_crc)                                                               // Ignore messages with invalid CRC
   {
    if (ENABLE_VERBOSE) serialPrint((char *)"Invalid Message CRC\n");
    digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED
    return false;
   }

   uint8_t swapChar = msgBuffer[6];
   msgBuffer[6] = 0;
   uint32_t clientOUI = strtoul((char*)msgBuffer, NULL, 16);  // Convert first 6 hex chars into a 24bit OUI
   if (clientOUI != serverOUI)                                // Ignore messages from clients that don't belong to us, (ie. different OUI)
   {
    if (ENABLE_VERBOSE) serialPrint((char *)"Client does not belong to this organisation\n");
    digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED
    return false;
   }
   msgBuffer[6] = swapChar;

   swapChar = msgBuffer[16];
   msgBuffer[16] = 0;
   uint64_t clientUID = strtoull((char*)msgBuffer, NULL, 16);  // Convert first 16 hex chars into a 64bit OUI+UID
   if (findClient(clientUID) == NUM_CLIENTS)                   // Ignore messages from clients not managed by this gateway
   {
    if (ENABLE_VERBOSE) serialPrint((char *)"Client not managed by this gateway\n");
    digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED
    return false;
   }
   msgBuffer[16] = swapChar;

//    uint8_t data[2];
//    sprintf((char*)data, "%02X", DEFAULT_SERVER_ADDRESS);  // Acknowledge the client's data packet with this server's address
//
//    if (manager.sendtoWait(data, sizeof(data), from))
//    {
//   digitalWrite(LED_BUILTIN, LOW);
//
//    if (ENABLE_VERBOSE) {serialPrint(serialbuf, "Reply:   \"%s\"", true, (char*)data);}
//    }
//    else if (ENABLE_VERBOSE) serialPrint((char*)"Reply Failed");
   if (!ENABLE_VERBOSE) {serialPrint((char*)msgBuffer, true);}  // Echo Data Packet out Serial Port: <OUI><UID>:<TEMP>:<RH>:<CRC>  eg: "0004A30B001A531C:123.45:123.45:7E"
  }
  else if (ENABLE_VERBOSE) serialPrint((char *)"SERVER: Invalid Packet");

  if (ENABLE_VERBOSE) serialPrint((char *)"");
 }
 else
 {
  digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED
  return false;
 }

 digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED

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
//   Returns:      bool, (true if Successful, false if Fail)                                                          //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        If serial data overflows message buffer, the buffer pointer is reset to start                      //
//                 Packets have an initial command character, followed by optional data, an 8-bit CRC and             //
//                 terminated with a  Line Feed character                                                             //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool serialRead()
{
 while (Serial.available() > 0)
 {
  serialData[serialPtr] = Serial.read();
  if (serialData[serialPtr] == SERIAL_END_CHAR) serialReceived = true;
  else {if (++serialPtr >= SERIAL_PACKET_LENGTH) serialPtr = 0;}
 }

 if (serialReceived)
 {
  serialData[serialPtr] = 0;  // Append a NULL to the end of the string

  uint8_t calc_crc = crc((void*)serialData, serialPtr-2);                             // Calculate message CRC
  uint8_t msg_crc = (uint8_t)strtoul((char*)(&(serialData[serialPtr-2])), NULL, 16);  // Convert last 2 hex chars into an 8bit CRC
  if (calc_crc != msg_crc)                                                            // Ignore messages with invalid CRC
  {
   if (ENABLE_VERBOSE) {serialPrint("SERVER: Invalid Serial Message CRC", true);}
   else {serialPrintf(serialbuf, "%s%s", true, true, ERROR_INVALID_CRC, ":");}  // eg. "0002:6E"
   serialPtr = 0;
   serialReceived = false;
   return false;
  }

  switch (serialData[0])
  {
   case 'T':  // Timestamp "T0123456789<CRC><LF>"  eg. "T156231193407"
   {
//    uint32_t timestamp = strtoul(&(serialData[1]), NULL, 10);  // Convert remaining chars into a 32-bit timestamp
    uint32_t timestamp = convertStrToUint(&(serialData[1]));  // Convert remaining chars into a 32-bit timestamp
    rtc.setY2kEpoch(timestamp);

//    if (ENABLE_VERBOSE)
//    {
//     char pbuf[255];
////     serialPrint((char*)(&(serialData[1])));
//     serialPrintf(pbuf, "Timestamp = %u", (uint32_t)(strtoul(&(serialData[1]), NULL, 10)));
//     serialPrintf(pbuf, "Timestamp = %u", convertStrToUint(serialData));

    getDateTime();
//     serialPrintf(pbuf, "rtc.getY2kEpoch() = %u", rtc.getY2kEpoch());
//     serialPrintf(pbuf, "rtc.getDay() = %u", dateTime.day);
//     serialPrintf(pbuf, "rtc.getMonth() = %u", dateTime.month);
//     serialPrintf(pbuf, "rtc.getYear() = %u", dateTime.year);
//     serialPrintf(pbuf, "rtc.getHours() = %u", dateTime.hour);
//     serialPrintf(pbuf, "rtc.getMinutes() = %u", dateTime.minute);
//     serialPrintf(pbuf, "rtc.getSeconds() = %u", dateTime.second);
//    }
    if (ENABLE_VERBOSE) {serialPrint("SERVER: Date/Time Successfully Set", true);}
    else {serialPrintf(serialbuf, "%s%s", true, true, MESSAGE_RCVD_OK, ":");}  // eg. "1000:32"
   }
   break;

   case 'C':  // Clear Client List, eg "CA4<LF>"
   {
/*
    clientListFileExists = SerialFlash.exists(CLIENT_LIST_FILENAME);
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
*/
    for (uint8_t i=0; i<NUM_CLIENTS; i++) clientList[i] = 0;

    if (ENABLE_VERBOSE) {serialPrint("SERVER: Client List Cleared", true);}
    else {serialPrintf(serialbuf, "%s%s", true, true, MESSAGE_RCVD_OK, ":");}  // eg. "1000:32"
   }
   break;

   case 'P':  // Enable/Disable Pairing Mode eg. "P197<LF>", "P0C9<LF>"
   {
    switch(serialData[1])  // First digit in command packet
    {
     case '0':
      pairingEnabled = false;
      if (ENABLE_VERBOSE) {serialPrint("SERVER: Pairing Disabled", true);}
      else {serialPrintf(serialbuf, "%s%s", true, true, MESSAGE_RCVD_OK, ":");}  // eg. "1000:32"
     break;
     case '1':
      pairingEnabled = true;
      if (ENABLE_VERBOSE) {serialPrint("SERVER: Pairing Enabled", true);}
      else {serialPrintf(serialbuf, "%s%s", true, true, MESSAGE_RCVD_OK, ":");}  // eg. "1000:32"
     break;
    }
   }
   break;

   default:
   {
    if (ENABLE_VERBOSE) {serialPrint("SERVER: Unknown Command", true);}
    else {serialPrintf(serialbuf, "%s%s", true, true, ERROR_UNKNOWN_MESSAGE, ":");}  // eg. "0003:AA"
   }
   break;
  }

  serialPtr = 0;
  serialReceived = false;
 }

 return true;
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

 serverOUI = strtoul(OUI, NULL, 16);

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
 digitalWrite(LED_BUILTIN, HIGH);  // Turn On LED

 // USB Serial Port Initialisation
 Serial.begin(BAUD_RATE);
 while (WAIT_SERIAL && !Serial) ; // Wait for serial port to be available (This will cause it to hang when not connected to a USB serial port)
 if (ENABLE_VERBOSE) {delay(15000);}     // Wait 15 seconds to allow enough time to connect a serial monitor

 // RTC initialisation
 rtc.begin();
 rtc.setTime(dateTime.hour, dateTime.minute, dateTime.second);
 rtc.setDate(dateTime.day, dateTime.month, dateTime.year);

 // Serial Flash Initialisation, (also initialises SPI)
 if (ENABLE_VERBOSE) {serialPrint("SERVER: Initialising Serial Flash", true);}
 serialFlashOk = SerialFlash.begin();
 if (!serialFlashOk) {if (ENABLE_VERBOSE) {serialPrint("SERVER: Failed to Initialise Serial Flash", true);} else {serialPrint(ERROR_FLASH_MEMORY, true, true);}}

 // I2C Initialisation
 Wire.begin();

 // LoRa Radio Initialisation
 radioInitialised = manager.init();
 if (!radioInitialised)
 {
  if (ENABLE_VERBOSE) {serialPrint("SERVER: Failed to Initialise LoRa Radio");}
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

 digitalWrite(LED_BUILTIN, LOW);  // Turn OFF LED

 if (ENABLE_VERBOSE)
 {
  uint32_t low = uid & 0xFFFFFFFF, high = (uid >> 32) & 0xFFFFFFFF;
  serialPrintf(serialbuf, "SERVER: Starting Server: UID = 0x%08lX%08lX\n", true, false, high, low);
 }
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
