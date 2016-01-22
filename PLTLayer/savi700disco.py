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
#		python savi700disco.py
#
# Have fun! ;-)
#

import msvcrt
import clr
import time
pltlayer = clr.AddReference("PLTLayer")

from System import String, Int32
from System.Collections.Generic import List

# variables/constants
sleepsecslong = 1.5
sleepsecsshort = 0.5

p = String("Welcome to PLTLayer Python Savi 700 Disco")		# test that Python for .NET is working
print (p)

print (pltlayer.FullName)	# test we have the PLTLayer.dll Assembly loaded

from Plantronics.EZ.API import PLTLayer	# import the PLTLayer class

plt = PLTLayer.Instance		# get the PLYLayer singleton instance

# define an event handler for all Plantronics events:
def handler(source, args):
	print ('PltEvent Occured! EventType=', args.EventType, '(' , args.EventTypeStr , ')')
	if args.MyParams is not None:
		for item in args.MyParams:
			print (item)
	
plt.PltEvent += handler		# add our event handler

plt.setup("PLTLayerPythonSavi700Disco")		# register our application with Plantronics

plt.lineoff(1)
plt.lineoff(2)
plt.lineoff(3)

print ("Press a key to start disco...")

msvcrt.getch()  #wait for key press

plt.lineon(1)
time.sleep(sleepsecslong)
plt.lineon(2)
time.sleep(sleepsecslong)
plt.lineon(3)
time.sleep(sleepsecslong)

plt.linehold(2, False)
time.sleep(sleepsecslong)
plt.linehold(1, False)

time.sleep(sleepsecslong)
plt.linehold(1, True)
time.sleep(sleepsecslong)
plt.lineoff(3)
time.sleep(sleepsecslong)
plt.lineoff(2)
time.sleep(sleepsecslong)
plt.lineoff(1)

time.sleep(sleepsecslong)
plt.lineon(3)
time.sleep(sleepsecslong)
plt.lineoff(3)
time.sleep(sleepsecsshort)
plt.lineon(2)
time.sleep(sleepsecslong)
plt.lineoff(2)
time.sleep(sleepsecsshort)
plt.lineon(1)
time.sleep(sleepsecslong)
plt.lineoff(1)

time.sleep(sleepsecslong)
print ("Hope you enjoyed it! Press a key to quit...")

msvcrt.getch()  #wait for another key press

plt.lineoff(1)
plt.lineoff(2)
plt.lineoff(3)

time.sleep(1)	 #wait for 1 second, *warning* without this Hub will crash!

plt.PltEvent -= handler

plt.shutdown()

print ("Bye")