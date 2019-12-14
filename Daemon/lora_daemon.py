#!/usr/bin/env python
# lora_daemon.py
# Periodically checks for LoRa Commands and Responses via file system "pigeon holes"
# Runs as a daemon on system startup
# Daemon Commands: service lora_daemon [start|stop|restart|status]
#------------------------------------------------------------

# Import Libraries
import os
import daemon
import glob
import time
import serial
import common
import device
#------------------------------------------------------------

try:
 commandPath = "/dev/shm/lora"
 loopDelay = 100
 commDev = "cds_lora"
 commPort = "/dev/" + commDev
 baudRate = 115200
 loraController = None
except:
 common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: config] ERROR START >>>")
 common.logging.debug("<<<  [lora_daemon.py: config] ERROR END >>>\n")
#------------------------------------------------------------

# Remote Command Format: "C, P:1, P:0, T:1562311934"
def process(command):
#{
 global commDev, commandPath, loraController

 try:
 #{
  if (command == ''): return False

  if (common.debug != 0): os.system("echo \"[" + common.getlogtimestamp() + "] [lora_daemon.py: process]  command = " + command + "\" >> " + common.SystemLog)

  if (os.system("sudo ls /dev | grep '" + commDev + "' &> /dev/null") == 0):  # Check if RFID Controller is connected and powered up
  #{
   if (loraController is not None and loraController.open()):  # Connect to RFID Controller
   #{
    try:
    #{
     if command[0] == "C":  # Clear Client List - Command: "C"
     #{
      result = loraController.clearClientList()  # Clear Client List

      with open(commandPath + "/IN/" + str(time.time()), 'w+') as handle:  # Write command response
       handle.write("C:" + ("1" if (result) else "0"))

      return result
     #}
     elif command[0] == "T" and command[1] == ":" and command[2] <> "":  # Set Date Time - Commands: "T:1562311934"
     #{
      params = command.split(':')
      if (len(params) <> 2): return False

      result = loraController.setDateTime(params[1])  # Set Date Time

      with open(commandPath + "/IN/" + str(time.time()), 'w+') as handle:  # Write command response
       handle.write("T:" + ("1" if (result) else "0"))

      return result
     #}
     elif command[0] == "P" and command[1] == ":":  # Enable/Disable Pairing
     #{
      if command[2] == "0":  # Disable Pairing
      #{
       result = loraController.enablePairing(0)

       with open(commandPath + "/IN/" + str(time.time()), 'w+') as handle:  # Write command response
        handle.write("P:0:" + ("1" if (result) else "0"))

       return result
      #}
      if command[2] == "1":  # Enable Pairing
      #{
       result = loraController.enablePairing(1)

       with open(commandPath + "/IN/" + str(time.time()), 'w+') as handle:  # Write command response
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
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: process] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: process] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

def getCommandFiles():
#{
 global commandPath

 try:
 #{
#  files = sorted(glob.glob(commandPath + "/OUT/*"))
  files = glob.glob(commandPath + "/OUT/*")
  files.sort(key=os.path.getmtime)

  return files
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: getCommandFiles] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: getCommandFiles] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

def update():
#{
 try:
 #{
  commandFiles = getCommandFiles()

  if (type(commandFiles) == type(False)): return False
  if (len(commandFiles) == 0): return False

  for commandFile in commandFiles:
  #{
   with open(commandFile, 'r') as handle:
   #{
    command = handle.read()
    process(command.strip())
    os.remove(commandFile)
   #}
  #}

  return True
 #}
 except:
 #{
  common.logging.exception("<<< [" + common.getlogtimestamp() + "] [lora_daemon.py: update] ERROR START >>>")
  common.logging.debug("<<<  [lora_daemon.py: update] ERROR END >>>\n")
 #}

 return False
#}
#------------------------------------------------------------

def init():
#{
 global loraController, commandPath, commDev, commPort, baudRate

 try:
 #{
  result = (os.system("sudo mkdir -p '" + commandPath + "/IN' &> /dev/null") == 0)
  if (result): result = (os.system("sudo mkdir -p '" + commandPath + "/OUT' &> /dev/null") == 0)

  if (result): result = (os.system("sudo ls /dev | grep '" + commDev + "' &> /dev/null") == 0)  # Check if RFID Controller is connected and powered up

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
  if (common.debug != 0): os.system("echo \"[" + common.getlogtimestamp() + "] [lora_daemon.py: loop]  RFID Control Daemon Started\" >> " + common.SystemLog)

  while (not init()):
  #{
   common.delay(loopDelay)
  #}

  while(1):
  #{
   update()
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
