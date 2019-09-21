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

 if (str == NULL || !Serial) return result;

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
 if (str == NULL || format == NULL || !Serial) return -1;

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     getUID                                                                                             //
//   Description:  This function returns a 64-bit UID read from the 24AA02E64 IC                                      //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    char* _uidstr                                                                                      //
//   Returns:      uint64_t                                                                                           //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        It replaces the 24AA02E64 OUI with a custom one, (OUI).                                            //
//                 SAMD21 ID based version duration is 511 us                                                         //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
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

 if (VERBOSE) {sprintf(_uidstr, "%s%02X%02X%02X%02X%02X", OUI, buf[0], buf[1], buf[2], buf[3], buf[4]);}

 return strtoull(_uidstr, NULL, 16);
}
*/
void getSAMDID(uint32_t* SAMDID)
{
 uint32_t id_addresses[4] = {ID_ADDR_0, ID_ADDR_1, ID_ADDR_2, ID_ADDR_3};

 for (uint8_t i=0; i<4; i++) {SAMDID[i] = *((uint32_t*)(id_addresses[i]));}
}

uint64_t getUID(char* _uidstr)
{
 uint32_t id_words[4];

 getSAMDID(id_words);

 char samd_id_buf[33];
 sprintf(samd_id_buf, "%8lX%8lX%8lX%8lX", id_words[0], id_words[1], id_words[2], id_words[3]);
 Serial.print("SAMD Id: 0x");
 Serial.println(samd_id_buf);

 volatile uint32_t samd_id_hash = hash(id_words, 4);
// volatile uint32_t samd_id_hash = md_hash(id_words, 4);
 char samd_id_hash_buf[9];
 sprintf(samd_id_hash_buf, "%8lX", samd_id_hash);
// sprintf(samd_id_hash_buf, "%8lX", md_hash(id_words, 4));
// sprintf(samd_id_hash_buf, "%8lX", md_hash(samd_id_buf, strlen(samd_id_buf)));
 Serial.print("MD Hash: 0x");
 Serial.println(samd_id_hash_buf);

 sprintf(_uidstr, "%s%02X%s", OUI, 0, samd_id_hash_buf);

 return strtoull(_uidstr, NULL, 16);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Function:     printChipId                                                                                        //
//   Description:  Print the MCU's 128-Bit UID                                                                        //
//                 -------------------------------------------------------------------------------------------------  //
//   Arguments:    N/A                                                                                                //
//   Returns:      void                                                                                               //
//                 -------------------------------------------------------------------------------------------------  //
//   Notes:        SAM D21 Family Datasheet Section 10.3.3 Serial Number                                              //
//                 Each device has a unique 128-bit serial number which is a concatenation of four 32-bit words       //
//                 contained at the following addresses:                                                              //
//                 Word 0: 0x0080A00C                                                                                 //
//                 Word 1: 0x0080A040                                                                                 //
//                 Word 2: 0x0080A044                                                                                 //
//                 Word 3: 0x0080A048                                                                                 //
//                 The uniqueness of the serial number is guaranteed only when using all 128 bits.                    //
//   Known Bugs:   None                                                                                               //
//   ---------------------------------------------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void printChipId()
//{
// uint32_t val1 = *((uint32_t*)ID_ADDR_0);
// uint32_t val2 = *((uint32_t*)ID_ADDR_1);
// uint32_t val3 = *((uint32_t*)ID_ADDR_2);
// uint32_t val4 = *((uint32_t*)ID_ADDR_3);

 /*
 volatile uint32_t val1, val2, val3, val4;
 volatile uint32_t *ptr1 = (volatile uint32_t *)0x0080A00C;
 val1 = *ptr1;
 volatile uint32_t *ptr = (volatile uint32_t *)0x0080A040;
 val2 = *ptr;
 ptr++;
 val3 = *ptr;
 ptr++;
 val4 = *ptr;
*/
// Serial.print("chip id: 0x");
// char buf[33];
// sprintf(buf, "%8X%8X%8X%8X", val1, val2, val3, val4);
// Serial.println(buf);
// Serial.print("SHA1:");
// Serial.println(sha1(buf));
// Serial.print("HASH:");
// sprintf(buf, "%8X", md_hash(buf, strlen(buf)));
// Serial.println(buf);
//}

uint32_t hash(const uint32_t* data, size_t data_length)
{
 uint8_t* str = (uint8_t*)data;
 uint8_t length = 4 * data_length;
 uint32_t hash = 0xAAAAAAAA;

 for (uint8_t i = 0; i < length; ++str, ++i)
 {
  hash ^= ((i & 1) == 0) ? ((hash <<  7) ^ (*str) * (hash >> 3)) : (~((hash << 11) + ((*str) ^ (hash >> 5))));
 }

 return hash;
}

uint32_t md_mix(uint32_t message_block, uint32_t internal_state)
{
 return (internal_state * message_block) ^ ((internal_state << 3) + (message_block >> 2));
}

uint32_t md_hash(const uint32_t* message, size_t message_length)
{
 uint32_t internal_state = 0xA5A5A5A5; // IV: A magic number

 // Loop over the message 32-bits at-a-time
 for (uint8_t i=0; i<message_length; i++)
 {
  internal_state = md_mix(message[i], internal_state);
 }

 return internal_state;
}
/*
uint32_t md_hash(const char* message, size_t message_length)
{
 uint32_t internal_state = 0xA5A5A5A5; // IV: A magic number
 uint32_t message_block = 0;

 // Loop over the message 32-bits at-a-time
 while (message_length >= 4)
 {
  message_block = *((uint32_t*)message);
// memcpy((void*)(&message_block), (void*)message, sizeof(uint32_t));

 internal_state = md_mix(message_block, internal_state);

 message_length -= sizeof(uint32_t);
 message        += sizeof(uint32_t);
}

// Are there any remaining bytes?
if (message_length)
{
 message_block = *((uint32_t*)message);
// memcpy((void*)(&message_block), (void*)message, message_length);
 internal_state = md_mix(message_block, internal_state);
}

 return internal_state;
}
*/
