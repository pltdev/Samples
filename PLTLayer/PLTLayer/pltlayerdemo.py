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
#	  - And added remaining event decoding same as in C# demo
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

from Plantronics.EZ.API import PLTLayer, PltEventType

MY_APP_NAME = "PLTLayerPythonTest"

# 5. Handle events received from Plantronics
# 
# This event handler method is called by PLTLayer whenever a 
# Plantronics event occurs, e.g. device events, call state events etc.
# 
# By examining the "args.EventType" and "args.MyParams" parameters, your
# app can see what the event was and additional information about the event.
# For example, in the case of EventType SerialNumber, you get 2 MyParams
# strings, the first is the Serial Number (also known as Genes id), the second 
# contains whether it is a Base or Headset serial.
#
# Parameters:
# source: The object that has sent us the event, in this case 
#   the PLTLayer object.
# args: The arguments for this event, containing the EventType 
#   and for some EventTypes a list of useful parameters (as strings) that 
#   are specific to the EventType.
def handler(source, args):
	# Example processing of incoming events/parameters to inform my app
	# what is happening with Plantronics:
	#
	# BASIC SOFTPHONE CALL CONTROL EVENTS:
	#
	if args.EventType == PltEventType.CallAnswered:
		print ("Plantronics answered a Softphone Call:\r\n"
				"Call Id: ", args.MyParams[0], "\r\n"
				"Call Source: ", args.MyParams[1])		
		# is the call in my app?
		if args.MyParams[1] == MY_APP_NAME:
			print ("\r\nTHIS CALL ID: ", args.MyParams[0], " IS IN MY APP!: ", MY_APP_NAME)
			# TODO: here you would answer the call in your app
	elif args.EventType == PltEventType.CallEnded:
		print ("Plantronics ended a Softphone Call:\r\n"
				"Call Id: ", args.MyParams[0], "\r\n"
				"Call Source: ", args.MyParams[1])		
		# is the call in my app?
		if args.MyParams[1] == MY_APP_NAME:
			print ("\r\nTHIS CALL ID: ", args.MyParams[0], " IS IN MY APP!: ", MY_APP_NAME)
			# TODO: here you would end the call in your app
	elif args.EventType == PltEventType.Muted:
		print ("Plantronics was muted")
		# TODO: syncronise with your app's mute feature
	elif args.EventType == PltEventType.UnMuted:
		print ("Plantronics was un-muted")
		# TODO: syncronise with your app's mute feature

	# ADVANCED SOFTPHONE CALL CONTROL EVENTS:
	#
	elif args.EventType == PltEventType.OnCall:
		print ("Plantronics went on Softphone Call:\r\n"
			"Call Id: ", args.MyParams[0], "\r\n"
			"Call Source: ", args.MyParams[1], "\r\n"
			"Is Incoming?: ", args.MyParams[2], "\r\n"
			"Call State: ", args.MyParams[3])
		# is the call in my app?
		if args.MyParams[1] == MY_APP_NAME:
			print ("\r\nTHIS CALL ID: ", args.MyParams[0], " IS IN MY APP!: ", MY_APP_NAME)
			# OnCall event is for information only.
			# you should use CallAnswered event to answer the call in your app
		else:
			print ("\r\nInfo, call id: ", args.MyParams[0], " is not in my app.")
			# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.NotOnCall:
		print ("Plantronics ended a Softphone Call:\r\n"
			"Call Id: ", args.MyParams[0], "\r\n"
			"Call Source: ", args.MyParams[1])
		# is the call in my app?
		if args.MyParams[1] == MY_APP_NAME:
			print ("\r\nTHIS CALL ID: ", args.MyParams[0], "WAS IN MY APP!: ", MY_APP_NAME)
			# NotOnCall event is for information only.
			# you should use CallEnded event to end the call in your app
		else:
			print ("\r\nInfo, call id: ", args.MyParams[0], " is not in my app.")
			# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.CallSwitched:
		print ("Plantronics switched a Softphone Call")
		# this event is for information only, the callid of activated call will
		# be available via a CallAnswered event
	elif args.EventType == PltEventType.CallRequested:
		print ("Plantronics dialpad device requested (dialed) a Softphone Call to this contact:\r\n"
			"Email: ", args.MyParams[0], "\r\n"
			"FriendlyName: ", args.MyParams[1], "\r\n"
			"HomePhone: ", args.MyParams[2], "\r\n"
			"Id: ", args.MyParams[3], "\r\n"
			"MobilePhone: ", args.MyParams[4], "\r\n"
			"Name: ", args.MyParams[5], "\r\n"
			"Phone: ", args.MyParams[6], "\r\n"
			"SipUri: ", args.MyParams[7], "\r\n"
			"WorkPhone: ", args.MyParams[8])
		# TODO: optional: here you would dial an outgoing call in your app

	# PLANTRONICS MULTI-LINE DEVICE EVENTS (e.g. Savi 700 Series):
	#
	elif args.EventType == PltEventType.MultiLineStateChanged:
		print ("Plantronics multiline state changed: \r\n"
			"       PC Line: Active?: ", args.MyParams[0], ", Held?: ", args.MyParams[1], "\r\n"
			"   Mobile Line: Active?: ", args.MyParams[2], ", Held?: ", args.MyParams[3], "\r\n"
			"Deskphone Line: Active?: ", args.MyParams[4], ", Held?: ", args.MyParams[5])
		# TODO: optional syncronise with your app's agent availability feature

	# PLANTRONICS "GENES ID" FEATURE EVENTS (DEVICE SERIAL NUMBERS):
	#
	elif args.EventType == PltEventType.SerialNumber:
		print ("Plantronics Genes ID (Serial Number) was received:\r\n"
			"Serial Number: ", args.MyParams[0], "\r\n"
			"Serial Type: ", args.MyParams[1])
		# TODO: optional syncronise with your app's agent tracking system
		# (e.g. asset tracking or used to apply user personalised settings, i.e. on shared workstation)

	# PLANTRONICS "CONTEXTUAL INTELLIGENCE FEATURE EVENTS:
	#
	elif args.EventType == PltEventType.PutOn:
		print ("Plantronics was put on")
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.TakenOff:
		print ("Plantronics was taken off")
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.Near:
		print ("Plantronics in wireless range: NEAR")
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.Far:
		print ("Plantronics in wireless range: FAR")
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.InRange:
		print ("Plantronics came into wireless range")
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.OutOfRange:
		print ("Plantronics went out of wireless range")
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.Docked:
		print ("Plantronics was docked")
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.UnDocked:
		print ("Plantronics was un-docked")
		# TODO: optional syncronise with your app's agent availability feature

	# PLANTRONICS DEVICE INFORMATION EVENTS:
	#
	elif args.EventType == PltEventType.Attached:
		print ("Plantronics was attached: Product Name"
			": ", args.MyParams[0], ", Product Id: ", args.MyParams[1])
		# TODO: optional: switch your app to headset audio mode when Plantronics attached
	elif args.EventType == PltEventType.Detached:
		print ("Plantronics was detached")
		# TODO: optional: switch your app to non-headset audio mode when Plantronics detached
	elif args.EventType == PltEventType.CapabilitiesChanged:
		print ("Plantronics device discovered capabilities have changed, as follows:\r\n"
			"HasDocking: ", args.MyParams[0], "\r\n"
			"HasMobCallerId: ", args.MyParams[1], "\r\n"
			"HasMobCallState: ", args.MyParams[2], "\r\n"
			"HasMultiline: ", args.MyParams[3], "\r\n"
			"HasProximity: ", args.MyParams[4], "\r\n"
			"HasWearingSensor: ", args.MyParams[5], "\r\n"
			"IsWireless: ", args.MyParams[6], "\r\n"
			"ProductId: ", args.MyParams[7])
		# TODO: optional: use this information to know what features/events to expect from
		# Plantronics device in your app

	# MOBILE CALL CONTROL EVENTS (e.g. Voyager Legend, Voyager Edge, Calisto 620):
	#
	elif args.EventType == PltEventType.OnMobileCall:
		print ("Plantronics went on Mobile Call:\r\n"
			"Is Incoming?: ", args.MyParams[0], "\r\n"
			"Call State: ", args.MyParams[1])
		# TODO: optional syncronise with your app's agent availability feature
	elif args.EventType == PltEventType.MobileCallerId:
		print ("Plantronics reported Mobile Caller Id (remote party phone number):\r\n"
			"Mobile Caller Id?: ", args.MyParams[0])
		# TODO: optional syncronise with your app's contacts database / CRM system
	elif args.EventType == PltEventType.NotOnMobileCall:
		print ("Plantronics ended a Mobile Call")
		# TODO: optional syncronise with your app's agent availability feature

	# OTHER DEVICE EVENTS:
	#
	elif args.EventType == PltEventType.BaseButtonPressed:
		print ("Plantronics base button pressed: button id"
			": ", args.MyParams[0])
		# BaseButtonPressed event is for information only
		# Note: some devices will generate button events internally even when no
		# physical button is pressed.
	elif args.EventType == PltEventType.ButtonPressed:
		print ("Plantronics button pressed: button id"
			": ", args.MyParams[0])
		# ButtonPressed event is for information only
		# Note: some devices will generate button events internally even when no
		# physical button is pressed.
		
	# Example debug output to show ALL events/parameters (commented out)
	#print ('PltEvent Occured! EventType=', args.EventType, '(' , args.EventTypeStr , ')')
	#if args.MyParams is not None:
	#	for item in args.MyParams:
	#		print (item)

# 1. Setup Plantronics
plt = PLTLayer.Instance
quit = False

plt.PltEvent += handler

plt.setup(MY_APP_NAME)

# User input functions...
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

# 4. Send app commands to Plantronics.
#
# The command and parameters will be read from the user on the command line,
# then the appropriate method of the PLTLayer object "plt" will be called.
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

# 2. Main application loop
while (quit is not True):
	NextUserCommand()

time.sleep(1)	 #wait for 1 second, *warning* without this Hub will crash!

# 3. Shutdown Plantronics
plt.PltEvent -= handler
plt.shutdown()
print ("Bye")
