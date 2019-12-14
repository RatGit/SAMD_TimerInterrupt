#!/usr/bin/env python
# common.py
# Common functions and global variables
#------------------------------------------------------------

import os
import sys
#import tty
#import termios
import commands
import logging
import datetime
import time
import glob
import serial
#------------------------------------------------------------

if (os.name) == 'nt': import msvcrt, winsound  # Windows
else: import tty, termios, select, atexit      # Linux
#------------------------------------------------------------

def strtoint(s, b=10):
#{
 try:
 #{
  s = s.strip()
  return int(s, b) if (s) else 0
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: strtoint] ERROR START >>>")
  logging.debug("<<<  [common.py: strtoint] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def strtofloat(s):
#{
 try:
 #{
  s = s.strip()
  return float(s) if (s) else 0.0
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: strtofloat] ERROR START >>>")
  logging.debug("<<<  [common.py: strtofloat] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def getlogtimestamp():
#{
 try:
 #{
  return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
 #}
 except:
 #{
  logging.exception("<<< [common.py: getlogtimestamp] ERROR START >>>")
  logging.debug("<<<  [common.py: getlogtimestamp] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

try:
#{
 ScriptPath = os.path.dirname(os.path.realpath(sys.argv[0]))

 ErrorLog = ScriptPath + "/logs/error.log"
 SystemLog = ScriptPath + "/logs/rfid_daemon.log"

 logging.basicConfig(filename=ErrorLog, level=logging.DEBUG,)

 debug = 0

 version = 1000

 currenttimestamp = 0
 daysofweek = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday', 'sunday']
 currentday = daysofweek[datetime.datetime.today().weekday()]
 now = datetime.datetime.now()
 currenttime = now.hour*60 + now.minute

 daemon = 1
#}
except:
#{
 logging.exception("<<< [" + getlogtimestamp() + "] [common.py: config] ERROR START >>>")
 logging.debug("<<<  [common.py: config] ERROR END >>>\n")
#}
#------------------------------------------------------------

def isFloat(s):
#{
 try:
 #{
  float(s)
  return True
 #}
 except ValueError:
 #{
  return False
 #}
#}
#------------------------------------------------------------

def beep(frequency=750, length=250, repetitions=1):
#{
 try:
 #{
  if (os.name) == 'nt':  # Windows
  #{
   for i in range(int(round(repetitions))): winsound.Beep(frequency, length)
  #}
  else:  # Linux
  #{
   os.system("beep -f " + str(int(round(frequency))) + " -l " + str(int(round(length))) + " -r " + str(int(round(repetitions))))
  #}
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: beep] ERROR START >>>")
  logging.debug("<<<  [common.py: beep] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def testcommand(command):
#{
 try:
 #{
  command = command.strip()
  if (command == ''): return False
  return (commands.getoutput(command + ' > /dev/null 2>&1 && echo 1 || echo 0') == '1')
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: testcommand] ERROR START >>>")
  logging.debug("<<<  [common.py: testcommand] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def delay(ms):
#{
 if (ms <= 0): return
 if (ms > 3600000): ms = 3600000  # 1 hour max delay
 time.sleep(1.0*ms/1000.0)
#}
#------------------------------------------------------------

def wait(ms=0, increment=100, timeout=30000):  # Default Maximum of 30 secs in 100msec increments
#{
 global debug

 try:
 #{
  timer = 0
  while (timer < ms):
  #{
   delay(increment)
   timer = timer + increment
   if (timer >= timeout): return False
  #}
  return True
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: wait] ERROR START >>>")
  logging.debug("<<<  [common.py: wait] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def getcurrentday():
#{
 global currentday, daysofweek

 try:
 #{
  currentday = daysofweek[datetime.datetime.today().weekday()]
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: getcurrentday] ERROR START >>>")
  logging.debug("<<<  [common.py: getcurrentday] ERROR END >>>\n")
 #}
#}
#------------------------------------------------------------

def waitforprocess(processname, wait=100, timeout=30000):  # Default Maximum of 30 secs
#{
 global debug, SystemLog

 try:
 #{
  if (debug != 0): os.system("echo \"[" + getlogtimestamp() + "] [common.py: waitforprocess]  processname = " + processname + "\" >> " + SystemLog)
  timer = 0
  while (not checkforprocess(processname)):
  #{
   delay(wait)
   timer = timer + wait
   if (timer >= timeout): return False
  #}
  return True
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: waitforprocess] ERROR START >>>")
  logging.debug("<<<  [common.py: waitforprocess] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def checkforprocess(processname, useregex=False):
#{
 try:
 #{
  return (os.system("ps -ef | grep -v 'grep' | grep " + ("-iP " if useregex else "") + "'" + processname + "'") == 0)
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: checkforprocess] ERROR START >>>")
  logging.debug("<<<  [common.py: checkforprocess] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def getchar(block=False):  # Returns a single character from standard input
#{
 try:
 #{
  if (os.name) == 'nt':  # Windows
  #{
   if (block or msvcrt.kbhit()): return msvcrt.getch()
   return ''
  #}

  fd = sys.stdin.fileno()
  old_settings = termios.tcgetattr(fd)

  try:
  #{
   if (block): tty.setraw(sys.stdin.fileno())
   else: tty.setcbreak(sys.stdin.fileno())

   if (block or select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], [])): ch = sys.stdin.read(1)
   else: ch = ''
  #}
  finally: termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

  return ch
 #}
 except:
 #{
  logging.exception("<<< [" + getlogtimestamp() + "] [common.py: getchar] ERROR START >>>")
  logging.debug("<<<  [common.py: getchar] ERROR END >>>\n")
 #}
 return False
#}
#------------------------------------------------------------

def listComPorts(removeLinks = False):
#{
 if (os.name == 'nt'): ports = ['COM%s' % (i + 1) for i in range(256)]
 else:
 #{
  ports = commands.getoutput("ls -l /dev | grep -iP 'tty(?:usb|acm)' | sed -E 's@^.+?\\s+[0-9\\:]+\\s+(.+)$@\\1@'").split("\n")

  linkedPorts = []
  for index, port in enumerate(ports):
  #{
   link = port.split("->")
   if (len(link) == 2):
   #{
    linkedPorts.append(link)
    if (not removeLinks): ports[index] = link[0].strip()
   #}
   ports[index] = "/dev/" + ports[index]
  #}

  for link in linkedPorts:
  #{
   if (removeLinks):
   #{
    ports.remove("/dev/" + link[1].strip())
    ports.remove("/dev/" + link[0] + "->" + link[1])
   #}
   else: ports.remove("/dev/" + link[1].strip())
  #}
 #}

 result = []
 for port in ports:
 #{
  try:
  #{
   s = serial.Serial(port)
   s.close()
   result.append(port)
  #}
  except (OSError, serial.SerialException): pass
 #}

 return result
#}
#------------------------------------------------------------
# serial.tools.list_ports.comports()
# [('/dev/ttyS3', 'ttyS3', 'n/a'),
#  ('/dev/ttyS2', 'ttyS2', 'n/a'),
#  ('/dev/ttyS1', 'ttyS1', 'n/a'),
#  ('/dev/ttyS0', 'ttyS0', 'n/a'),
#  ('/dev/ttyUSB0',
#   'Linux Foundation 1.1 root hub ',
#   'USB VID:PID=0403:6001 SNR=A1017L9P')]

class KBHit:
#{
 def __init__(self):  # Creates a KBHit object that you can call to do various keyboard things.
 #{
  if (os.name == 'nt'): pass
  else:
  #{
   # Save the terminal settings
   self.fd = sys.stdin.fileno()
   self.new_term = termios.tcgetattr(self.fd)
   self.old_term = termios.tcgetattr(self.fd)

   # New terminal setting unbuffered
   self.new_term[3] = (self.new_term[3] & ~termios.ICANON & ~termios.ECHO)
   termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.new_term)

   # Support normal-terminal reset at exit
   atexit.register(self.set_normal_term)
  #}
 #}
 #------------------------------------------------------------

 def set_normal_term(self):  # Resets to normal terminal.  On Windows this is a no-op.
 #{
  if (os.name == 'nt'): pass
  else: termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.old_term)
 #}
 #------------------------------------------------------------

 def getch(self):  # Returns a keyboard character after kbhit() has been called. Should not be called in the same program as getarrow().
 #{
  if os.name == 'nt': return msvcrt.getch().decode('utf-8')
  else: return sys.stdin.read(1)
 #}
 #------------------------------------------------------------

 def getarrow(self):  # Returns an arrow-key code after kbhit() has been called. Codes are  0 : up, 1 : right, 2 : down, 3 : left. Should not be called in the same program as getch().
 #{
  if (os.name == 'nt'):
  #{
   msvcrt.getch() # skip 0xE0
   c = msvcrt.getch()
   vals = [72, 77, 80, 75]
  #}
  else:
  #{
   c = sys.stdin.read(3)[2]
   vals = [65, 67, 66, 68]
  #}

  return vals.index(ord(c.decode('utf-8')))
 #}
 #------------------------------------------------------------

 def kbhit(self):  # Returns True if keyboard character was hit, False otherwise.
 #{
  if (os.name == 'nt'): return msvcrt.kbhit()
  else:
  #{
   dr,dw,de = select.select([sys.stdin], [], [], 0)
   return dr != []
  #}
 #}
 #------------------------------------------------------------
#}
#------------------------------------------------------------
