# Test python script to interact with Plantronics SDK
# Lewis.Collins@Plantronics.com 21st Aug 2014
#
# Pre-requisites:
# --
# 1. http://www.plantronics.com/software
#    - download Latest Hub for Windows
#
# 2. https://www.python.org/downloads/
#    - download and install Python 3.4.1 for Windows 64-bit or 32-bit
#
# 3. www.lfd.uci.edu/~gohlke/pythonlibs/#pythonnet
#    - download and run installer: 64-bit: pythonnet-2.0.0.win-amd64-py3.4.exe, 32-bit: pythonnet-2.0.0.win32-py3.4.exe
#     (this is the Python for .NET module)
#
# 4. PLTLayer.dll
#    - The Minimalist Plantronics .NET API - download from Developer City on Wave
#
# **NOTE**: ensure you match system architectures 64-bit or 32-bit!!!
# The "python" and "Python for .NET" must match, also the .NET Assembly must
# target same CPU: for 64-bit should target "x64 or Any", and for 32-bit should target "x86"
#
# More info:
# --
# http://pythonnet.sourceforge.net/readme.html#getting_started
# - this is the reference guide for Python for .NET
#
# How to run:
# --
#   Once you have the above pre-requisites installed:
#   From Command Prompt type:
#
#		python pltlayerdemo.py
#
# pltlayerdemo.py
# 
# This is a test program to show the features of the PLTLayer API
#
# PLTLayer is a new minimalist API for controlling and interacting with
# Plantronics devices from your applications.
# For more details refer to the PLTLayer project.
# 
# Author: Lewis Collins
# 
# Contact: Lewis.Collins@Plantronics.com
# 
# VERSION HISTORY:
# ********************************************************************************
# Version 1.0.0.1:
# Date: 26th August 2014
# Compatible with Plantronics Hub version(s): 3.0.50718.1966
# Changed by: Lewis Collins
#   Changes:
#     - Added remaining user commands to bring into line
#		  with the C# demo PLTLayerTestApp.cs, v1.0.0.1
#
# Version 1.0.0.0:
# Date: 22nd August 2014
# Compatible with Plantronics Hub version(s): 3.0.50718.1966
# Changed by: Lewis Collins
#   Changes:
#     - Initial version
# ********************************************************************************
#
# Have fun! ;-)
#

import msvcrt
import clr
import time
pltlayer = clr.AddReference("PLTLayer")

from System import String, Int32
from System.Collections.Generic import List
p = String("Welcome to PLTLayer test")		# test that .NET is working
print (p)

print (pltlayer.FullName)

from Plantronics.EZ.API import PLTLayer

# 1. Setup Plantronics
plt = PLTLayer.Instance
quit = False

def handler(source, args):
	print ('PltEvent Occured! EventType=', args.EventType, '(' , args.EventTypeStr , ')')
	if args.MyParams is not None:
		for item in args.MyParams:
			print (item)
	
plt.PltEvent += handler

plt.setup("PLTLayerPythonTest")

# 2. Main application loop
def GetUserInput(prompt = "Enter Command (or type help) > "):
	print
	var = input(prompt)
	return var

def ShowValidCommands():
	print
	print ("Valid commands:")
	print ("--")
	print ("on, ring, ans, off, muteon, muteoff, hold, resume, lineon, lineoff,\r\nlinehold, lineresume, dialmob, ansmob, rejmob, endmob, getgenes, help, quit")

def safe_cast(val, to_type, default=None):
    try:
        return to_type(val)
    except ValueError:
        return default
		
def ReadCallInfo():
	isincoming = safe_cast(GetUserInput("Enter call direction \r\n(integer, 1 = incoming, 2 = outgoing) : "),int,1) != 2
	callid = safe_cast(GetUserInput("Enter call id (integer)\r\nor press Enter to generate call id automatically : "),int,-1)
	contactname = GetUserInput("Enter contact name (string)\r\nor press Enter to skip : ")
	return isincoming, callid, contactname

def ReadCallInfoRing():
	callid = safe_cast(GetUserInput("Enter call id (integer)\r\nor press Enter to generate call id automatically : "),int,-1)
	contactname = GetUserInput("Enter contact name (string)\r\nor press Enter to skip : ")
	return callid, contactname

def ReadCallInfoOrLast():
	callid = safe_cast(GetUserInput("Enter call id (integer)\r\nor press Enter for last call : "),int,-1)
	return callid

def ReadCallInfoOff():
	callid = safe_cast(GetUserInput("Enter call id (integer)\r\nor press Enter to end all calls : "),int,-1)
	return callid

def ReadCallInfoCallId():
	callid = safe_cast(GetUserInput("Enter call id (integer)\r\nor press Enter to generate call id automatically : "),int,-1)
	return callid

def ReadLineInfo():
	callid = safe_cast(GetUserInput("Enter line number (integer, 1 = PC, 2 = Mobile, 3 = Desk) : "),int,1)
	return callid
	
def ReadNumberToDial():
	numbertodial = GetUserInput("Enter number to dial (string) : ")
	return numbertodial
	
def NextUserCommand():
	command = GetUserInput()
	isincoming = True
	callid = 0
	contactname = ""
	line = 0
	numbertodial = ""
	global quit
	# BASIC SOFTPHONE CALL CONTROL FEATURE EXAMPLES:
	#
	if command == "on":
		isincoming, callid, contactname = ReadCallInfo()
		# Notify Plantronics our app has a new incoming or outgoing call
		if callid == -1:
			# generate a call id automatically
			if len(contactname)>0:
				callid = plt.on(isincoming, contactname)
			else:
				callid = plt.on(isincoming, "")		# note: need to pass contactname of "" to avoid it confusing with on(PLTLine line) override!
			print ("\r\nAutogenerated call id was: ", callid)
		else:
			# use the call id specified by our app
			plt.on(isincoming, callid, contactname)
	elif command == "ring":
		callid, contactname = ReadCallInfoRing()
		# Notify Plantronics our app has a new incoming call
		if callid == -1:
			# generate a call id automatically
			if len(contactname)>0:
				callid = plt.ring(contactname)
			else:
				callid = plt.ring("")	# note: need to pass contactname of "" because Python for .NET module does not understand .NET default parameters
			print ("\r\nAutogenerated call id was: ", callid)
		else:
			# use the call id specified by our app
			plt.ring(callid, contactname)
	elif command == "ans":
		callid = ReadCallInfoOrLast()
		# Notify Plantronics an incoming call was answered in our app
		if callid == -1:
			# just answer the last call
			plt.ans()
		else:
			# use the call id specified by our app
			plt.ans(callid)
	elif command == "off":
		callid = ReadCallInfoOff()
		# Notify Plantronics a call was ended in our app
		if callid == -1:
			# end all calls
			plt.off()
		else:
			# end specific call
			plt.off(callid)
	elif command == "muteon":
		plt.mute(True) # go on mute
	elif command == "muteoff":
		plt.mute(False) # go off mute
		
	elif command == "hold":
		callid = ReadCallInfoOrLast()
		# Notify Plantronics a call was held in our app
		if callid == -1:
			# just hold the last call
			plt.hold(True)
		else:
			# use the call id specified by our app
			plt.hold(callid, True)
	elif command == "resume":
		callid = ReadCallInfoOrLast()
		# Notify Plantronics a call was resumed in our app
		if callid == -1:
			# just resume the last call
			plt.hold(False)
		else:
			# use the call id specified by our app
			plt.hold(callid, False)

	# PLANTRONICS MULTI-LINE DEVICE EXAMPLES (e.g. Savi 700 Series):
	elif command == "lineon":
		line = ReadLineInfo()
		# activate the chosen line
		plt.lineon(line)
	elif command == "lineoff":
		line = ReadLineInfo()
		# de-activate the chosen line
		plt.lineoff(line)
	elif command == "linehold":
		line = ReadLineInfo()
		# hold the chosen line
		plt.hold(line, True)
	elif command == "lineresume":
		line = ReadLineInfo()
		# resume/un-hold the chosen line
		plt.hold(line, False)

	# MOBILE CALL CONTROL EXAMPLES (e.g. Voyager Legend, Voyager Edge, Calisto 620):
	elif command == "dialmob":
		numbertodial = ReadNumberToDial()
		# use mobile to dial the specified number
		plt.dialmob(numbertodial)
	elif command == "ansmob":
		# answer ringing incoming call on mobile
		plt.ansmob()
	elif command == "rejmob":
		# reject ringing incoming call on mobile
		plt.rejmob()
	elif command == "endmob":
		# end call on mobile
		plt.endmob()

	# PLANTRONICS "GENES" FEATURE (GET DEVICE SERIAL NUMBERS):
	elif command == "getgenes":
		# request the genes id's of base and headset devices
		plt.getgenes()
		
	elif command == "quit":
		quit = True
	elif command == "help":
		ShowValidCommands()
	else:
		print ("unknown command")
		ShowValidCommands()
	return

while (quit is not True):
	NextUserCommand()

time.sleep(1)	 #wait for 1 second, *warning* without this Hub will crash!

# 3. Shutdown Plantronics
plt.PltEvent -= handler
plt.shutdown()
print ("Bye")
