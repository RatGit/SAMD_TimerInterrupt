////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//  PROJECT:      Wireless Sensor Controller                                                                          //
//                                                                                                                    //
//  MODULE:       common.h                                                                                            //
//                                                                                                                    //
//  DESCRIPTION:  Low Power LoRa Packet Radio Client/Server common header file.                                       //
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
//  ----------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     serialPrint                                                                                        //
//   Description:  Wrapper function for Serial.print and Serial.println                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    const char* str                                                                                    //
//                 bool addLF                                                                                         //
//   Returns:      int                                                                                                //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     serialPrintf                                                                                       //
//   Description:  Wrapper function for sprintf, Serial.print and Serial.println                                      //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* str                                                                                          //
//                 const char* format                                                                                 //
//                 bool addLF                                                                                         //
//                 variadic arguments                                                                                 //
//   Returns:      int                                                                                                //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     getTimestampStr                                                                                    //
//   Description:  This function returns a formatted timestamp string. (= seconds since 1st January 2000, 00:00:00)   //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* dateTimeStr                                                                                  //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     getDateTime                                                                                        //
//   Description:  This function updates the global dateTime struct from the RTC.                                     //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* dateTimeStr                                                                                  //
//                 const char* format                                                                                 //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     getDateTimeStr                                                                                     //
//   Description:  This function updates the global dateTime struct and returns a formatted date time string as       //
//                 ddd, d mmm yyyy hh:mm:ss                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* dateTimeStr                                                                                  //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Year must be greater than 2000                                                                     //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     createDatagram                                                                                     //
//   Description:  Write the 64-Bit OUI+UID, Timestamp (secs since Power On) and last SNR.                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    None                                                                                               //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        Called in "radioHandshake"                                                                         //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _common_h_
#define _common_h_


//////////////////////////////////////
//  Included Modules and Functions  //
//////////////////////////////////////

#include "Arduino.h"

#include <stdarg.h>

#include <SerialFlash.h>
#include <RTCZero.h>
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>


/////////////////////////
//  Macro Definitions  //
/////////////////////////

#define VERBOSE true         // Enable/Disable verbose output
#define ALLOW_PRINT true  // Enable printing of diagnostic messages out of USB serial port
#define USE_SERIAL true   // Enable/Disable use of serial port

#define BAUD_RATE 115200

#define EUI64_CHIP_ADDRESS 0x50
#define UID_ADDRESS 0xFB     // Address of first byte in Unique Identifier
#define UID_LENGTH 5         // Number of bytes in Unique Identifier

#define RADIO_WAIT 3000
#define RADIO_FREQ 916.8
#define RADIO_POWER 23
#define DEFAULT_SERVER_ADDRESS 253
#define PAIRING_ADDRESS  RH_PAIRING_ADDRESS  // RH_BROADCAST_ADDRESS
#define PACKET_LENGTH 27  // = 24bit OUI + 40bit UID + ':' + 12bit Temperature + ':' + 12bit Relative Humidity + ':' + 8bit CRC  (All in hexadecimal), eg: "0004A30B001A531C:DF7:234:7E"
#define CLIENT_ACK_TIMEOUT 5000
#define NUM_RETRIES 10

#define Serial SerialUSB  // Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)


////////////////////////
//  Type Definitions  //
////////////////////////

struct DateTime
{
 uint8_t year, month ,day, hour, minute, second;  // second: 0-59, minute: 0-59, hour: 0-23, day: 0-6 (Sunday = 0), month: 1-12, year: 4 digit year
};


///////////////////////////////////////////////
//  External Global Variables and Constants  //
///////////////////////////////////////////////

// Chip Select Pin Numbers
extern const int radioDio0;
extern const int radioDio1;
extern const int radioDio2;
extern const int radioReset;
extern const int radioChipSelect;

extern const int flashChipSelect;

// Change these values to set the current initial time
extern const uint8_t seconds;
extern const uint8_t minutes;
extern const uint8_t hours;

// Change these values to set the current initial date
extern const uint8_t day;
extern const uint8_t month;
extern const uint8_t year;

// 24-Bit Organisationally Unique Identifier
extern uint32_t serverOUI;

// 64-Bit Unique Identifier
extern uint64_t uid;
extern char uidstr[];

// Initialise global DateTime Struct
extern const char* monthNames[];
extern const char* dayNames[];
extern DateTime dateTime;

// Create an RTC object
extern RTCZero rtc;

// Create Radio Driver and Manager
extern uint8_t msgBuffer[];    // Buffer to hold received message (Don't put this on stack???)
extern RH_RF95 radio;          // Singleton instance of the radio driver: Rocket Scream Mini Ultra Pro with the RFM95W
extern bool radioInitialised;  // Flag is set to the manager.init() result during setup


/////////////////////////////
//  Function Declarations  //
/////////////////////////////

uint8_t crc(void *data_pointer, uint16_t number_of_bytes);
int serialPrint(const char *str=NULL, bool addLF=true);
int serialPrintf(char* str=NULL, const char* format=NULL, bool addLF=true, ...);
void getTimestampStr(char* dateTimeStr);
void getDateTime();
void getDateTimeStr(char* dateTimeStr);
void createDatagram(uint8_t* data);
void alarmMatch();
void setup();
void loop();


#endif
