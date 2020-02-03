#!/usr/bin/env python
# lora_daemon.py
# Periodically checks for LoRa Commands and Responses via file system "pigeon holes"
# Runs as a daemon on system startup
# Daemon Commands: service lora_daemon [start|stop|restart|status]
#
# Comms Between the Gateway GUI and the CD Server:
# ================================================
# Data and Responses from the CDS Server:
#     CDS Server => Daemon (Serial Comms)
#     Daemon => Gateway GUI (File System):   "/dev/shm/lora/DAT", "/dev/shm/lora/RSP"
#            => CDS API Server (Internet)
#
# Commands from The Gateway GUI:
#     Gateway GUI => Daemon (File System):   "/dev/shm/lora/CMD"
#     Daemon => CDS Server (Serial Comms)
#            => CDS API Server (Internet)
#
# Note: If the GUI is expecting a Response from a Command then it needs to check the "/dev/shm/lora/RSP" folder for the incoming response data file
#------------------------------------------------------------

# Import Libraries
import os
import commands
import daemon
import glob
import time
import serial
import common
import device
import sqlite3
#------------------------------------------------------------

try:
 commandPath = "/dev/shm/lora"
 commandFiles = None
 databasePath = '/home/pi/gateway.db';
 cdsApiUrl = 'http://cds.netsoft-projects.com:1234'
 loopDelay = 100
 commDev = "cds_lora"
 commPort = "/dev/" + commDev
 baudRate = 115200
 loraController = None
except:
 common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: config] ERROR START >>>")
 common.logging.debug("<<<  [lora_daemon.py: config] ERROR END >>>\n")
#------------------------------------------------------------

def getCommands():
#{
 global commandPath, commandFiles

 try:
 #{
#  commandFiles = sorted(glob.glob(commandPath + "/CMD/*"))
  commandFiles = glob.glob(commandPath + "/CMD/*")
  commandFiles.sort(key=os.path.getmtime)

  return True
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: getCommands] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: getCommands] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

def processCommands():
#{
 global commandFiles

 try:
 #{
  if (len(commandFiles) == 0): return True

  for commandFile in commandFiles:
  #{
   with open(commandFile, 'r') as handle:
   #{
    command = handle.read()
    processCommand(command.strip())
    os.remove(commandFile)
   #}
  #}

  return True
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: processCommands] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: processCommands] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

# LoRa Server Commands: C, P:1, P:0, T:1562311934
# LoRa Server Responses: C:0|1, P:1:0|1, P:0:0|1, T:0|1
def processCommand(command):
#{
 global commDev, commandPath, loraController

 try:
 #{
  if (command == ''): return False

  if (common.debug != 0): os.system("echo \"[" + common.getlogtimestamp() + "] [lora_daemon.py: processCommand]  command = " + command + "\" >> " + common.SystemLog)

  if (os.system("sudo ls /dev | grep '" + commDev + "' &> /dev/null") == 0):  # Check if LoRa Controller is connected and powered up
  #{
   if (loraController is not None and loraController.open(device.COM_TIMEOUT)):  # Connect to LoRa Controller
   #{
    try:
    #{
     if command[0] == "C":  # Clear Client List - Command: "C"
     #{
      result = loraController.clearClientList()  # Clear Client List

      with open(commandPath + "/RSP/" + str(time.time()), 'w+') as handle:  # Write command response
       handle.write("C:" + ("1" if (result) else "0"))

      return result
     #}
     elif command[0] == "T" and command[1] == ":" and command[2] <> "":  # Set Date Time - Commands: "T:1562311934"
     #{
      params = command.split(':')
      if (len(params) <> 2): return False

      result = loraController.setDateTime(params[1])  # Set Date Time

      with open(commandPath + "/RSP/" + str(time.time()), 'w+') as handle:  # Write command response
       handle.write("T:" + ("1" if (result) else "0"))

      return result
     #}
     elif command[0] == "P" and command[1] == ":":  # Enable/Disable Pairing
     #{
      if command[2] == "0":  # Disable Pairing
      #{
       result = loraController.enablePairing(0)

       with open(commandPath + "/RSP/" + str(time.time()), 'w+') as handle:  # Write command response
        handle.write("P:0:" + ("1" if (result) else "0"))

       return result
      #}
      if command[2] == "1":  # Enable Pairing
      #{
       result = loraController.enablePairing(1)

       with open(commandPath + "/RSP/" + str(time.time()), 'w+') as handle:  # Write command response
        handle.write("P:1:" + ("1" if (result) else "0"))

       return result
      #}
      else: return False
     #}
     else: return False
    #}
    finally:
    #{
     loraController.close()
    #}

    return True
   #}
  #}

  return False
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: processCommand] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: processCommand] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

# Write any received Data Packets to Files in the "DAT" Pidgeon Hole
def getData():
#{
 global loraController, commandPath

 try:
 #{
  if (loraController is not None and loraController.open(1)):  # Connect to LoRa Controller
  #{
   try:
   #{
    while loraController.readData(True):  # Read Serial Data from CDS Server
    #{
     with open(commandPath + "/DAT/" + str(time.time()), 'w+') as handle:  # Data Packet: <OUI><UID>:<TEMP>:<RH>:<CRC>  eg. "0004A30B001A531C:123.45:123.45:7E"
      handle.write(loraController.response)                                # Pairing Response:  eg. "1003:0004A30B001A531C"
    #}
   #}
   finally:
   #{
    loraController.close()
   #}

   return True
  #}

  return False
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: getData] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: getData] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

# Send any Unsynced Sensor Data to the CDS API Server
def processData():
#{
 global cdsApiUrl, databasePath

 try:
 #{
  try: db = sqlite3.connect(databasePath)
  except: return False

  cur = db.cursor()
  cur.execute("SELECT sensorid, PRINTF('%016X', sensorid), timestamp, temperature, humidity, accx, accy, accz FROM data WHERE synced = 0")

  rows = cur.fetchall()

  updatecur = None
  for row in rows:
  #{
   command = 'sudo curl -s -H \\"Content-Type: application/json\\" -H \\"Accept: application/vnd.cds.100+application/json\\" -H \\"Accept-Encoding: gzip\\" --request POST --url \\"' + cdsApiUrl + '/data/create\\" --data \'{\\"sensorid\\":\\"' + str(row[1]) + '\\",\\"timestamp\\":' + str(row[2]) + ',\\"temperature\\":' + str(row[3]) + ',\\"humidity\\":' + str(row[4]) + ',\\"accx\\":' + str(row[5]) + ',\\"accy\\":' + str(row[6]) + ',\\"accz\\":' + str(row[7]) +'}\''
   if (common.debug != 0): os.system("echo \"[" + common.getlogtimestamp() + "] [lora_daemon.py: processData]  command = " + command + "\" >> " + common.SystemLog)
   response = commands.getoutput('sudo curl -s -H "Content-Type: application/json" -H "Accept: application/vnd.cds.100+application/json" -H "Accept-Encoding: gzip" --request POST --url "' + cdsApiUrl + '/data/create" --data \'{"sensorid":"' + str(row[1]) + '","timestamp":' + str(row[2]) + ',"temperature":' + str(row[3]) + ',"humidity":' + str(row[4]) + ',"accx":' + str(row[5]) + ',"accy":' + str(row[6]) + ',"accz":' + str(row[7]) +'}\'')
   if (common.debug != 0): os.system("echo \"[" + common.getlogtimestamp() + "] [lora_daemon.py: processData]  response = " + response + "\" >> " + common.SystemLog)
#   if (response == '1' or response == '-1'):
   if (response <> ''):
   #{
    if (updatecur is None): updatecur = db.cursor()
    updatecur.execute('UPDATE data SET synced = 1 WHERE sensorid = \'' + str(row[0]) + '\' AND timestamp = ' + str(row[2]))
    db.commit()
   #}
  #}

#  if (not updatecur is None): updatecur.close()
#  cur.close()

  db.close()

  return True
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: processData] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: processData] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

def init():
#{
 global loraController, commandPath, commDev, commPort, baudRate

 try:
 #{
  result = (os.system("sudo mkdir -p '" + commandPath + "/CMD' &> /dev/null") == 0)
  if (result): result = (os.system("sudo mkdir -p '" + commandPath + "/RSP' &> /dev/null") == 0)
  if (result): result = (os.system("sudo mkdir -p '" + commandPath + "/DAT' &> /dev/null") == 0)

  if (result): result = (os.system("sudo chown -hR pi:pi '" + commandPath + "' &> /dev/null") == 0)

  if (result): result = (os.system("sudo ls /dev | grep '" + commDev + "' &> /dev/null") == 0)  # Check if LoRa Controller is connected and powered up

  if (result and loraController is None): loraController = device.LoRaController(commPort, baudRate, serial.PARITY_NONE, serial.STOPBITS_ONE, serial.EIGHTBITS, device.COM_TIMEOUT, device.COM_TIMEOUT, False, False, False)

  if (result): result = isinstance(loraController, device.LoRaController)

  return result
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: init] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: init] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def loop():
#{
 global loopDelay

 try:
 #{
  if (common.debug != 0): os.system("echo \"[" + common.getlogtimestamp() + "] [lora_daemon.py: loop]  LoRa Control Daemon Started\" >> " + common.SystemLog)

  while (not init()):
  #{
   common.delay(loopDelay)
  #}

  while(1):
  #{
   getCommands()
   processCommands()

   getData()
   processData();

   common.delay(loopDelay)
  #}
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: loop] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: loop] ERROR END >>>\n")
 #}
#}
#------------------------------------------------------------

def run():
#{
 try:
 #{
#  if (not init()):
#  #{
#   os.system("echo \"[" + common.getlogtimestamp() + "] [lora_daemon.py: run]  Unable to Initialise System\" | sudo tee -a '" + common.SystemLog + "'")
#   return
#  #}

  if (common.daemon == 1):
  #{
   with daemon.DaemonContext(): loop()
  #}
  else: loop()
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: run] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: run] ERROR END >>>\n")
 #}
#}
#------------------------------------------------------------

if __name__ == "__main__": run()
#------------------------------------------------------------
