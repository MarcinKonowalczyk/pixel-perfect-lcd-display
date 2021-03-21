# 
# Written by Marcin Konowalczyk
# Based on the makefile for Marlin firmware

## Operating system specific stuff

OS ?= $(shell uname -s)
ifneq ($(OS),Darwin)
  # TODO
  $(error Have not figured out the paths on other operating systems.)
endif

ifeq ($(OS), Darwin)
  # Assume Darwin (macOS)
  ARDUINO_INSTALL_DIR = /Applications/Arduino.app/Contents/Java
  ARDUINO_USER_DIR = ${HOME}/Documents/Arduino
  AVR_TOOLS_PATH = $(ARDUINO_INSTALL_DIR)/hardware/tools/avr/bin
endif

## Get the version of the arduino software installed

ARDUINO_DUMPVERSION = $(shell cat $(ARDUINO_INSTALL_DIR)/lib/version.txt)
ARDUINO_MAJ = $(shell echo $(ARDUINO_DUMPVERSION) | cut -d. -f1 -)
ARDUINO_MIN = $(shell echo $(ARDUINO_DUMPVERSION) | cut -d. -f2 -)
ARDUINO_PATCHLEVEL = $(shell echo $(ARDUINO_DUMPVERSION) | cut -d. -f3 -)
ARDUINO_VERSION_NUMBER = $(shell expr $(ARDUINO_MAJ) \* 10000 + $(ARDUINO_MIN) \* 100 + $(ARDUINO_PATCHLEVEL))

## Make sure we're in a root directory of the project
PWD = $(abspath .)
THIS_MAKEFILE = $(abspath $(firstword $(MAKEFILE_LIST)))
PROJECT_ROOT = $(patsubst %/,%,$(dir $(THIS_MAKEFILE)))

ifneq ($(PROJECT_ROOT),$(PWD))
  $(error Run make only form root directory)
endif

TARGET = $(notdir $(CURDIR))

## Directories and paths

BUILD_DIR = $(PROJECT_ROOT)/build
SRC_DIR = $(PROJECT_ROOT)/src

# Library files
LIB_SRC = wiring.c wiring_digital.c hooks.c
# LIB_SRC += wiring_analog.c wiring_digital.c wiring_shift.c
# LIB_SRC += WInterrupts.c

LIB_CXXSRC =
# LIB_CXXSRC += WMath.cpp WString.cpp Print.cpp SPI.cpp
# LIB_CXXSRC += Tone.cpp
LIB_CXXSRC += main.cpp

# Source files
SRC = $(shell find $(SRC_DIR) -name '*.c' -type f)
CXXSRC = $(shell find $(SRC_DIR) -name '*.cpp' -type f)

# Define all object files
# Put all arduino library files in an "./build/arduino_lib/" subfolder
OBJ = $(patsubst %.c, $(BUILD_DIR)/arduino_lib/%.o, $(LIB_SRC))
OBJ += $(patsubst %.cpp, $(BUILD_DIR)/arduino_lib/%.o, $(LIB_CXXSRC))

# Otherwise mirror the sidectory structure of "./src/"
OBJ += $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CSRC))
OBJ += $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXXSRC))

## Other stuff

CC = $(AVR_TOOLS_PATH)/avr-gcc
CXX = $(AVR_TOOLS_PATH)/avr-g++
OBJCOPY = $(AVR_TOOLS_PATH)/avr-objcopy
AVRDUDE = $(AVR_TOOLS_PATH)/avrdude
SIZE = $(AVR_TOOLS_PATH)/avr-size

# Programming support using avrdude. Settings and variables.
UPLOAD_PORT = /dev/cu.usbmodem14201
AVRDUDE_CONF = $(AVR_TOOLS_PATH)/../etc/avrdude.conf
AVRDUDE_FLAGS = -v -D -C $(AVRDUDE_CONF) -p atmega328p -c arduino
# AVRDUDE_FLAGS += -b 115200

# AR = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-ar
# SIZE = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-size
# NM = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-nm

VPATH =

# Most core arduino libraries
VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/cores/arduino

# Location of pins_arduino.h requred by Arduino.h
VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/variants/standard

# VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/libraries/SPI/src
# VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/libraries/SoftwareSerial/src
VPATH += $(ARDUINO_INSTALL_DIR)/libraries/LiquidCrystal/src

# VPATH += $(ARDUINO_INSTALL_DIR)/hardware/tools/avr/avr/include/avr/

## Collect compiler flags

CFLAGS =
CXXFLAGS =

# Pass definitions to the code
# 16MHz Clock Speed
DEFS = -D F_CPU=16000000UL
# DEFS += -D ARDUINO=$(ARDUINO_VERSION_NUMBER)
CFLAGS += $(DEFS)
CXXFLAGS += $(DEFS)

# Add include directories
CFLAGS += ${addprefix -I ,${VPATH}}
CXXFLAGS += ${addprefix -I ,${VPATH}}

# Set the C/CPP Standard level
CFLAGS += -std=gnu99
CXXFLAGS += -std=gnu++11

# Warnings
WARNINGS = -Wall -Wno-packed-bitfield-compat -Wno-pragmas -Wunused-parameter -Wfatal-errors -w -Wunused-function
CFLAGS += $(WARNINGS)
CXXFLAGS += $(WARNINGS)
CWARN += -Wstrict-prototypes

# Tuning
# TUNING = -fsigned-char -funsigned-bitfields -fno-exceptions -fshort-enums -ffunction-sections -fdata-sections
# CTUNING += -flto
CFLAGS += $(TUNING)
CXXFLAGS += $(TUNING)

# Optimisation level
OPTIMISATION = -Os
CFLAGS += $(OPTIMISATION)
CXXFLAGS += $(OPTIMISATION)

# MCU flags
CFLAGS += -mmcu=atmega328p
CXXFLAGS += -mmcu=atmega328p

# Extra flags
# CEXTRA = 
# CXXEXTRA = -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti

##
##
##

# Defalut target
all: size

# Remove the content of the build directory
clean:
	@ echo "Removing build directories"
	@ rm -rf $(BUILD_DIR)

# Create required build hierarchy if it does not exist
mkdir:
	@ mkdir -p $(dir $(OBJ))

build: mkdir elf hex bin

elf: $(BUILD_DIR)/$(TARGET).elf

##
## Build instructions
##

# Link: create ELF output file from library.
$(BUILD_DIR)/$(TARGET).elf: $(OBJ)
	@ echo " ELF  $(notdir $@) <- $(notdir $<)"
	@ $(CXX) -Wl,--gc-sections,--relax $(CXXFLAGS) -o $@ -L. $(OBJ) -lm

## Build objects from source
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@ echo " CC   $(notdir $@) <- $(notdir $<)"
	@ $(CC) -MMD -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@ echo " CXX  $(notdir $@) <- $(notdir $<)"
	@ $(CXX) -MMD -c $(CXXFLAGS) $< -o $@

# Silence warnings for library code (won't work for .h files, unfortunately)
# LIBWARN = -w -Wno-packed-bitfield-compat

# # Object files for Arduino libs will be created in $(BUILD_DIR)/arduino
$(BUILD_DIR)/arduino_lib/%.o: %.c
	@ echo " CC   $(notdir $@) <- $(notdir $<)"
	@ $(CC) -MMD -c $(CFLAGS) $(LIBWARN) $< -o $@

$(BUILD_DIR)/arduino_lib/%.o: %.cpp
	@ echo " CXX  $(notdir $@) <- $(notdir $<)"
	@ $(CXX) -MMD -c $(CXXFLAGS) $(LIBWARN) $< -o $@

# Automaticaly include the dependency files created by gcc
# This makes sure make sees changes in .h files too
-include ${patsubst %.o, %.d, ${OBJ}}

bin: $(BUILD_DIR)/$(TARGET).bin
hex: $(BUILD_DIR)/$(TARGET).hex

%.hex: %.elf
	@ echo " HEX  $(notdir $@)"
	@ $(OBJCOPY) -O ihex -R .eeprom $< $@

%.bin: %.elf
	@ echo " BIN  $(notdir $@)"
	@ $(OBJCOPY) -O binary -R .eeprom $< $@

upload: build
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(BUILD_DIR)/$(TARGET).hex:i -P $(UPLOAD_PORT)

# Display size fo the compiled elf file
size: build
	@ $(SIZE) --mcu=atmega2560 --format=avr $(BUILD_DIR)/$(TARGET).elf