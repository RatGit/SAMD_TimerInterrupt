////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//  PROJECT:      Wireless Sensor Controller                                                                          //
//                                                                                                                    //
//  MODULE:       server.h                                                                                            //
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

#ifndef _server_h_
#define _server_h_


/////////////////////////
//  Macro Definitions  //
/////////////////////////

//#ifndef __SAMD21G18A__
// #define __SAMD21G18A__     // Required for SerialFlash Library
//#endif


//////////////////////////////////////
//  Included Modules and Functions  //
//////////////////////////////////////

#include <util/SerialFlash_directwrite.h>


/////////////////////////
//  Macro Definitions  //
/////////////////////////

#define WAIT_SERIAL true     // Wait for serial port to be available (This will cause it to hang when not connected to a USB serial port)

#define NUM_CLIENTS 100

#define SERVER_OUI "0004A3"  // Have to apply for an OUI
#define SERIAL_END_CHAR 0x0a
#define SERIAL_PACKET_LENGTH 64


/////////////////////////////
//  Function Declarations  //
/////////////////////////////

uint64_t getUID(char* _uidstr);
void initClients();
uint8_t numClients();
uint8_t findClient(uint64_t clientUID);
uint8_t addClient(uint64_t clientUID);
uint8_t nextClient();
bool removeClient(uint64_t clientUID);
bool radioRead();
uint32_t convertStrToUint(char* str);
bool serialRead();


#endif
