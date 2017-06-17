#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
# 
#############################################################################

# source files in this project (main.cpp is automatically assumed)
SOURCES := fingerprintsensor/fingerprintsensor.cpp realtimeclock/realtimeclock.cpp

# header files in this project
HEADERS := fingerprintsensor/fingerprintsensor.hpp realtimeclock/realtimeclock.hpp

# other places to look for files for this project
SEARCH  := 

# settings for Arduino Due projects
TARGET            := arduino_due
SERIAL_PORT       ?= COM3
CONSOLE_BAUDRATE  := 9600

# the location of the ti software directory
TI-SOFTWARE       := C:/ti-software

# include Catch 
SEARCH            += $(TI-SOFTWARE)/Catch/include

# include hwlib
HWLIB             ?= $(TI-SOFTWARE)/hwlib
include           $(HWLIB)/Makefile.inc

# set BMPTK to the bmptk subdirectory
BMPTK := C:/ti-software/bmptk

# Defer to the bmptk/Makefile.inc to do the work
include $(BMPTK)/Makefile.inc