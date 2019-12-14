#!/usr/bin/env python
# device.py
# Class Framework for Serial Devices
################################################################################
import os
import serial
import common
################################################################################

# Global Constant Definitions
# Serial Comms
COM_TIMEOUT = 15   # Seconds
FLUSH_DELAY = 100  # Milliseconds

# Command Response Codes
COMMAND_SUCCESS = "1000"  # Serial command successful
COMMAND_FAIL    = "1001"  # Serial command failed
COMMAND_UNKNOWN = "1002"  # Unknown Serial Command
CLIENT_PAIRED   = "1003"  # Client Paired Successfully

OFF = 0
ON = 1
################################################################################

class Device:
#{
 def __init__(self, _port, _baudrate, _parity, _stopbits, _bytesize, _timeout, _write_timeout, _xonxoff, _rtscts, _dsrdtr):
 #{
  self.handle = None

  self.port          = _port
  self.baudrate      = _baudrate
  self.parity        = _parity
  self.stopbits      = _stopbits
  self.bytesize      = _bytesize
  self.timeout       = _timeout
  self.write_timeout = _write_timeout
  self.xonxoff       = _xonxoff
  self.rtscts        = _rtscts
  self.dsrdtr        = _dsrdtr
 #}
 ################################################################################

 def __del__(self):
 #{
  if (self.handle is not None):
  #{
   self.handle.close()  # Close serial port
  #}
 #}
 ################################################################################

 def open(self, commsTimeout):
 #{
  try:
  #{
#   if (self.handle is not None):
#   #{
#    self.clearCOMBuffers()
#    return True
#   #}

#   self.handle = serial.Serial(port=self.port, baudrate=self.baudrate, parity=self.parity, stopbits=self.stopbits, bytesize=self.bytesize, timeout=commsTimeout, writeTimeout=commsTimeout, xonxoff=self.xonxoff, rtscts=self.rtscts, dsrdtr=self.dsrdtr)
#   self.handle = serial.Serial(port=self.port, baudrate=self.baudrate, parity=self.parity, stopbits=self.stopbits, bytesize=self.bytesize, timeout=commsTimeout, write_timeout=commsTimeout, xonxoff=self.xonxoff, rtscts=self.rtscts, dsrdtr=self.dsrdtr)
   self.handle = serial.Serial(port=self.port, baudrate=self.baudrate, parity=self.parity, stopbits=self.stopbits, bytesize=self.bytesize, timeout=commsTimeout, xonxoff=self.xonxoff, rtscts=self.rtscts, dsrdtr=self.dsrdtr)
   if (self.handle is None): return False

   self.clearCOMBuffers()

   return True
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:open] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:open] ERROR END >>>\n")
  #}

  self.handle = None
  return False
 #}
 ################################################################################

 def close(self):
 #{
  try:
  #{
   if (self.handle is not None):
   #{
    self.handle.close()  # Close serial port
    self.handle = None
   #}

   return True
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:open] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:open] ERROR END >>>\n")
  #}

  return False
 #}
 ################################################################################

 def clearCOMBuffers(self, flushInput=True, flushOutput=True):
 #{
  global FLUSH_DELAY

  try:
  #{
   if (self.handle is None): return False

   common.delay(FLUSH_DELAY)
   if (flushInput): self.handle.flushInput()
   if (flushOutput): self.handle.flushOutput()

#   while (self.handle.readline() <> ''): pass  # Clear any messages from the COM buffer

   return True
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:clearCOMBuffers] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:clearCOMBuffers] ERROR END >>>\n")
  #}
  return False
 #}
 ################################################################################

 def calcCRC(self, message):
 #{
  try:
  #{
   number_of_bytes = len(message)
   if (number_of_bytes == 0): return ''

   crc8_result = 0
   ptr = 0

   while (ptr < number_of_bytes):
   #{
    temp1 = ord(message[ptr])
    ptr += 1

    for bit_counter in range(8, 0, -1):
    #{
     feedback_bit = (crc8_result & 0x01)
     crc8_result >>= 1
     if (feedback_bit ^ (temp1 & 0x01)): crc8_result ^= 0x8C
     temp1 >>= 1
    #}
   #}

   crc8_result =  ((hex(crc8_result).upper()).split('X'))[1].zfill(2)

   return crc8_result
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:calcCRC] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:calcCRC] ERROR END >>>\n")
  #}
  return ''
 #}
 ################################################################################

 def appendCRC(self, message):
 #{
  try:
  #{
   return (message + self.calcCRC(message))
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:appendCRC] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:appendCRC] ERROR END >>>\n")
  #}
  return message
 #}
 ################################################################################

 def appendCRCNL(self, message):
 #{
  try:
  #{
   return (message + self.calcCRC(message) + '\n')
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:appendCRCNL] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:appendCRCNL] ERROR END >>>\n")
  #}
  return message
 #}
 ################################################################################

 def checkCRC(self, message):
 #{
  try:
  #{
   message = message.strip()
   msgLength = len(message)
   if (msgLength < 3): return False

   return (self.calcCRC(message[:msgLength-2]) == message[msgLength-2:])
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:checkCRC] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:checkCRC] ERROR END >>>\n")
  #}
  return False
 #}
 ################################################################################

 def readResponse(self, debug = False):
 #{
  try:
  #{
   if (self.handle is None): return False

   response = self.handle.readline().strip()  # Read response

   if (debug): print("RESPONSE: " + response + " (CRC " + ("OK" if (self.checkCRC(response)) else 'ERROR: Received="' + response[len(response)-2:] + '" Expected="' + self.calcCRC(response[:len(response)-3]) + '"') + ")")

   if (self.checkCRC(response)): return response
   return False
#   if (type(self.checkCRC(response)) == type(False)): return False
#   return response
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:readResponse] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:readResponse] ERROR END >>>\n")
  #}

  return False
 #}
 ################################################################################

 def sendCommand(self, command, expectedResponse='', hasResponse=True, debug=False):
 #{
  try:
  #{
   if (self.handle is None): return False
   if (command == ''): return False
   expectedResponse = expectedResponse.strip()

   if (debug): print("COMMAND: " + self.appendCRC(command))
   self.handle.write(self.appendCRCNL(command))  # Append CRC and LF then send Command

   if (expectedResponse == ''):
   #{
    if (hasResponse):
    #{
     response = self.handle.readline().strip()  # Read response
     if (debug): print("MESSAGE: " + response[:len(response)-2])
     if (debug): print("RESPONSE: " + response + " (CRC " + ("OK" if (self.checkCRC(response)) else 'ERROR: Received="' + response[len(response)-2:] + '" Expected="' + self.calcCRC(response[:len(response)-2]) + '"') + ")")
     if (self.checkCRC(response)): return response
     return False
    #}
   #}
   else:
   #{
    response = self.handle.readline().strip()  # Read response
    if (debug): print("RESPONSE: " + response + " (" + ("OK" if (response == expectedResponse) else 'ERROR: Received="' + response + '" Expected="' + expectedResponse + '"') + ")")
    if (response == expectedResponse): return response
    return False
   #}

   return True
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:sendCommand] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:sendCommand] ERROR END >>>\n")
  #}
  return False
 #}
 ################################################################################
#}
################################################################################


class LoRaController(Device):
#{
 def __init__(self, _port, _baudrate, _parity, _stopbits, _bytesize, _timeout, _write_timeout, _xonxoff, _rtscts, _dsrdtr):
 #{
  Device.__init__(self, _port, _baudrate, _parity, _stopbits, _bytesize, _timeout, _write_timeout, _xonxoff, _rtscts, _dsrdtr)

  self.responseBuffer = []
  self.response = ''
 #}
 ################################################################################

 def __del__(self):
 #{
  Device.__del__(self)
 #}
 ################################################################################

 def readData(self, debug = False):
 #{
  try:
  #{
   if (self.handle is None): return False

   for char in self.handle.read():
   #{
    self.responseBuffer.append(char)
    if char == '\n':
    #{
     self.response = ''.join(self.responseBuffer)
     self.responseBuffer = []
    #}
   #}

   if (debug): print("RESPONSE: " + self.response + " (CRC " + ("OK" if (self.checkCRC(self.response)) else 'ERROR: Received="' + self.response[len(self.response)-2:] + '" Expected="' + self.calcCRC(self.response[:len(self.response)-3]) + '"') + ")")

   if (self.checkCRC(self.response)): return True
   else: self.response = ''

   return False
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: Device:readData] ERROR START >>>")
   common.logging.debug("<<<  [device.py: Device:readData] ERROR END >>>\n")
  #}

  return False
 #}
 ################################################################################

 def setDateTime(self, seconds, debug=False):  # Set Date Time: "T1562311934<CRC><LF>"  eg. "T156231193407"
 #{                                            # Command Successful Response: "1000:32"
  global COMMAND_SUCCESS

  try:
  #{
   if (self.handle is None): return False

   if (seconds <= 0): return False

   response = self.sendCommand('T'+str(seconds), '', True, debug)
   if (type(response) == type(False) and response == False): return False

   response = response.split(':')
   if (len(response) <> 2): return False

   if response[0] <> COMMAND_SUCCESS: return False

   return True
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: LoRaController:setDateTime] ERROR START >>>")
   common.logging.debug("<<<  [device.py: LoRaController:setDateTime] ERROR END >>>\n")
  #}

  return False
 #}
 ################################################################################

 def clearClientList(self, debug=False):  # Clear Client List, eg "CA4<LF>"
 #{                                       # Command Successful Response: "1000:32"
  global COMMAND_SUCCESS

  try:
  #{
   if (self.handle is None): return False

   response = self.sendCommand('C', '', True, debug)
   if (type(response) == type(False) and response == False): return False

   response = response.split(':')
   if (len(response) <> 2): return False

   if response[0] <> COMMAND_SUCCESS: return False

   return True
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: LoRaController:clearClientList] ERROR START >>>")
   common.logging.debug("<<<  [device.py: LoRaController:clearClientList] ERROR END >>>\n")
  #}

  return False
 #}
 ################################################################################

 def enablePairing(self, state, debug=False):  # Enable/Disable Pairing Mode eg. "P197<LF>", "P0C9<LF>"
 #{                                            # Command Successful Response: "1000:32"
  global COMMAND_SUCCESS

  try:
  #{
   if (self.handle is None): return False

   if (state < 0 or state > 1): return False

   response = self.sendCommand('P'+str(state), '', True, debug)
   if (type(response) == type(False) and response == False): return False

   response = response.split(':')
   if (len(response) <> 2): return False

   if response[0] <> COMMAND_SUCCESS: return False

   return True
  #}
  except:
  #{
   common.logging.exception("<<< [" + common.getlogtimestamp() + "] [device.py: LoRaController:enablePairing] ERROR START >>>")
   common.logging.debug("<<<  [device.py: LoRaController:enablePairing] ERROR END >>>\n")
  #}

  return False
 #}
 ################################################################################
#}
################################################################################
