////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                    //
//  PROJECT:      Wireless Sensor Controller                                                                          //
//                                                                                                                    //
//  MODULE:       common.cpp                                                                                          //
//                                                                                                                    //
//  DESCRIPTION:  Low Power LoRa Packet Radio Client/Server common source module.                                     //
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
//   Function:     crc                                                                                                //
//   Description:  Calculate an 8-Bit CRC                                                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    void* data_pointer                                                                                 //
//                 uint16_t number_of_bytes                                                                           //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
//                                                                                                                    //
//   Function:     ulltohex                                                                                           //
//   Description:  Convert uint64_t to Zero-Padded Hex String                                                         //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* str                                                                                          //
//                 uint64_t value                                                                                     //
//   Returns:      char* (NULL on Fail)                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
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

//////////////////////////////////////
//  Included Modules and Functions  //
//////////////////////////////////////

#include "common.h"


//////////////////////////////////////
//  Global Variables and Constants  //
//////////////////////////////////////

// Chip Select Pin Numbers
const int radioDio0 = 2;
const int radioDio1 = 6;
const int radioDio2 = 7;
const int radioReset = 3;
const int radioChipSelect = 5;

const int flashChipSelect = 4;

// Change these values to set the current initial time
const uint8_t seconds = 0;
const uint8_t minutes = 0;
const uint8_t hours = 0;

// Change these values to set the current initial date
const uint8_t day = 0;
const uint8_t month = 0;
const uint8_t year = 0;

// 24-Bit Organisationally Unique Identifier
uint32_t serverOUI;

// 64-Bit Unique Identifier
uint64_t uid;
char uidstr[] = "0000000000000000";

// Initialise global DateTime Struct
const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char* dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
DateTime dateTime = DateTime{0, 0, 0, 0, 0, 0};  // year, month ,day, hour, minute, second

// Create an RTC object
RTCZero rtc;

// Create Radio Driver and Manager
uint8_t msgBuffer[RH_RF95_MAX_MESSAGE_LEN];  // Buffer to hold received message (Don't put this on stack???)
RH_RF95 radio(radioChipSelect, radioDio0);   // Singleton instance of the radio driver: Rocket Scream Mini Ultra Pro with the RFM95W
bool radioInitialised;                       // Flag is set to the manager.init() result during setup


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     crc                                                                                                //
//   Description:  Calculate an 8-Bit CRC                                                                             //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    void* data_pointer                                                                                 //
//                 uint16_t number_of_bytes                                                                           //
//   Returns:      uint8_t                                                                                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t crc(void *data_pointer, uint16_t number_of_bytes)
{
 uint8_t temp1, bit_counter, feedback_bit, crc8_result = 0;
 uint8_t *ptr = (uint8_t *) data_pointer;

 while (number_of_bytes--)
 {
  temp1 = *ptr++;
  for (bit_counter=8; bit_counter; bit_counter--)
  {
   feedback_bit = (crc8_result & 0x01);
   crc8_result >>= 1;
   if (feedback_bit ^ (temp1 & 0x01))
   {
    crc8_result ^= 0x8c;
   }
   temp1 >>= 1;
  }
 }

 return (crc8_result);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     ulltohex                                                                                           //
//   Description:  Convert uint64_t to Zero-Padded Hex String                                                         //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* str                                                                                          //
//                 uint64_t value                                                                                     //
//   Returns:      char* (NULL on Fail)                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* ulltohex(char* str, uint64_t value)
{
 if (str == NULL) return NULL;

 uint16_t Word3, Word2, Word1, Word0;

 Word3 = (value & 0xFFFF000000000000) >> 48;
 Word2 = (value & 0x0000FFFF00000000) >> 32;
 Word1 = (value & 0x00000000FFFF0000) >> 16;
 Word0 = value & 0x000000000000FFFF;

 sprintf(str, "%04X", Word3);
 sprintf(&(str[4]), "%04X", Word2);
 sprintf(&(str[8]), "%04X", Word1);
 sprintf(&(str[12]), "%04X", Word0);

 return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int serialPrint(const char *str, bool addLF, bool addCRC)
{
 int result = -1;

 if (str == NULL || !USE_SERIAL || !Serial) return result;

 if (addCRC)
 {
  char* strbuff = new char[strlen(str)+3];
//  try
//  {
   sprintf(strbuff, "%s%02X", str, crc((void*)str, strlen(str)));  // Append calculated message CRC

   if (addLF) {result = Serial.println(strbuff);}
   else {result = Serial.print(strbuff);}
//  }
//  catch(...) {delete [] strbuff; strbuff = NULL;}
//  if (strbuff != NULL) {delete [] strbuff; strbuff = NULL;}
  delete [] strbuff;
 }
 else
 {
  if (addLF) {result = Serial.println(str);}
  else {result = Serial.print(str);}
 }

 return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int serialPrintf(char* str, const char* format, bool addLF, bool addCRC, ...)
{
 if (str == NULL || format == NULL || !USE_SERIAL || !Serial) return -1;

 va_list va;
 int result;

 va_start(va, addCRC);
 result = vsprintf(str, format, va);
 va_end(va);

 if (result >= 0) {result = serialPrint(str, addLF, addCRC);}

 return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     getTimestampStr                                                                                    //
//   Description:  This function returns a formatted timestamp string. (= seconds since 1st January 2000, 00:00:00)   //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* dateTimeStr                                                                                  //
//   Returns:      None                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getTimestampStr(char* dateTimeStr)
{
 sprintf(dateTimeStr, "%010lu", rtc.getY2kEpoch());  // 10 bytes required for a 100 year timestamp (in seconds)
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getDateTime()
{
 dateTime.second = rtc.getSeconds();
 dateTime.minute = rtc.getMinutes();
 dateTime.hour = rtc.getHours();
 dateTime.day = rtc.getDay();
 dateTime.month = rtc.getMonth();
 dateTime.year = rtc.getYear();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getDateTimeStr(char* dateTimeStr)
{
 getDateTime();
 sprintf(dateTimeStr, "%s, %u %s 20%02u %02u:%02u:%02u", dayNames[dateTime.day], dateTime.day, monthNames[dateTime.month], dateTime.year, dateTime.hour, dateTime.minute, dateTime.second);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     createDatagram                                                                                     //
//   Description:  Write the 64-Bit OUI+UID, Timestamp (secs since Power On) and last SNR.                            //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* - pointer to buffer to hold <OUI:UID><Timestamp:<SNR>                                        //
//   Returns:      char* - pointer to string holding <OUI:UID><Timestamp:<SNR> or NULL on fail                        //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        None                                                                                               //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* createDatagram(char* data)
{
 if (data == NULL) return NULL;

 strcpy((&(data[0])), uidstr);  // 16 characters required to contain 64-bit hexadecimal OUI:UID
 data[17] = ':';
 getTimestampStr(&(data[18]));  // 10 characters required to contain a 100 year decimal timestamp (in seconds)
 data[28] = ':';
 sprintf((&(data[29])), "[%d]", radio.lastSNR());  // 8-bit signed integer

 return data;
}
