////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//  PROJECT:      Wireless Sensor Controller                                                                          //
//                                                                                                                    //
//  MODULE:       client.h                                                                                            //
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

#ifndef _client_h_
#define _client_h_


//////////////////////////////////////
//  Included Modules and Functions  //
//////////////////////////////////////

#include <Adafruit_Si7021.h>
#include <SPIMemory.h>


/////////////////////////
//  Macro Definitions  //
/////////////////////////

//#define TEST_CURRENT  // Define to enable low power current measurement mode

#define LOW_POWER        // Define to enable low power mode

#define USE_SERIAL true       // Enable USB Serial Port
#define ENABLE_VERBOSE true   // Enable/Disable verbose output, (set to false for production)

#define LED_DEBUG false  // Blink LED in RadioHandshake()
#define LOOP_DELAY 1000  // Time in milliseconds to wait each loop (when in low power mode)
#define DEFAULT_CLIENT_ADDRESS 252
#define SERVER_ACK_TIMEOUT 5000
#define USE_Si7021  // Include code for the Si7021 Humidity/Temperature Sensor
#define NUM_PAIRINGS_SECONDS 6 //10  // Number of times to attempt to pair every 10 seconds, (on startup)
#define NUM_PAIRINGS_MINUTES 2 //10  // Number of times to attempt to pair every minute, (on startup)


//////////////////////////////////////
//  Global Variables and Constants  //
//////////////////////////////////////

bool isPaired;


/////////////////////////////
//  Function Declarations  //
/////////////////////////////

bool pair();
bool sendData();
bool radioHandshake();


#endif
