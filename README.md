Simple RFID Reader for Raspberry Pi
===================================

This is a simple database controlled application written in a very 
C-like C++ (sorry, I am a C programmer and cannot help myself ;-)).

This code will not work alone, the kernel on the PI needs to be changed 
to support reading from the SL030. Information on this can be found on 
my website: [bengreen.eu](http://www.bengreen.eu/fancyhtml/techiestuff/i2conraspberrypi.html)

The complicated programme that checks IDs and stuff requires SQLite for 
Raspberry Pi. Instructions on how to compile are available on my site. 
See [quickreference](http://www.bengreen.eu/fancyhtml/quickreference/).

OpenGL ES
---------

The directories `opengl_rpi` and `opengl_x11` are... guess what? 
the OpenGL ES on the Raspberry Pi and the implementation by Mesa 
when you are running x11.

Current status is incomplete but the code should give you something to 
base your ideas on.

Compile the `rcapp`
-------------------

Does not need anything complicated except a cross compiler!

If you have the cross compiler on your machine...

	armv6j-hardfloat-linux-gnueabi-gcc -I. -c readcard.c
	armv6j-hardfloat-linux-gnueabi-gcc -I. -c i2cbusses.c
	armv6j-hardfloat-linux-gnueabi-g++ rcapp.cpp readcard.o i2cbusses.o utilityfn.cpp -o rcapp

From the pi just use g++ :-)


