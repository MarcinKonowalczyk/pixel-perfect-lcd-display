# Bare Marlin Firmware Arduino Project Makefile
# Stripped down version
# 

OS ?= $(shell uname -s)
ifeq ($(OS), Darwin)
  # Assume Darwin (macOS)
  ARDUINO_INSTALL_DIR = /Applications/Arduino.app/Contents/Java
  ARDUINO_USER_DIR = ${HOME}/Documents/Arduino
  AVR_TOOLS_PATH = /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/
endif

ifndef ARDUINO_INSTALL_DIR
  $(error Figure out directory structure on windows.) # TODO
endif

ARDUINO_DUMPVERSION = $(shell cat $(ARDUINO_INSTALL_DIR)/lib/version.txt)
ARDUINO_MAJ := $(shell echo $(ARDUINO_DUMPVERSION) | cut -d. -f1 -)
ARDUINO_MIN := $(shell echo $(ARDUINO_DUMPVERSION) | cut -d. -f2 -)
ARDUINO_PATCHLEVEL := $(shell echo $(ARDUINO_DUMPVERSION) | cut -d. -f3 -)
ARDUINO_VER := $(shell expr $(ARDUINO_MAJ) \* 10000 + $(ARDUINO_MIN) \* 100 + $(ARDUINO_PATCHLEVEL))
ARDUINO_VERSION := $(shell expr $(ARDUINO_MAJ) \* 100 + $(ARDUINO_MIN))
# From boards.h
#define BOARD_EINSY_RAMBO 1203  // Einsy Rambo
HARDWARE_MOTHERBOARD = 1203

# Some stuff related to the microcontroller choice
# Set to arduino, ATmega2560 if not yet set.
HARDWARE_VARIANT = arduino
HARDWARE_SUB_VARIANT = mega
MCU              = atmega2560
PROG_MCU         = m2560
TOOL_PREFIX = avr
MCU_FLAGS   = -mmcu=$(MCU)
SIZE_FLAGS  = --mcu=$(MCU) -C

# Programmer configuration
UPLOAD_RATE        = 57600
AVRDUDE_PROGRAMMER = arduino

# On most linuxes this will be /dev/ttyACM0 or /dev/ttyACM1
UPLOAD_PORT        = /dev/null # /dev/cu.usbmodem14201

# Directory used to build files in.
BUILD_DIR = $(CURDIR)/build
# BUILD_DIR := $(realpath $(BUILD_DIR))

LIQUID_TWI2        = 0 # This defines whether Liquid_TWI2 support will be built
WIRE               = 0 # This defines if Wire is needed
TONE               = 1 # This defines if Tone is needed (i.e SPEAKER is defined in Configuration.h) # Disabling this (and SPEAKER) saves approximatively 350 bytes of memory.
U8GLIB             = 0 # This defines if U8GLIB is needed (may require RELOC_WORKAROUND)
TMC                = 0 # This defines whether to include the Trinamic TMCStepper library
NEOPIXEL           = 0 # This defines whether to include the AdaFruit NeoPixel library

# Program settings
CC = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-gcc
CXX = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-g++
OBJCOPY = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-objcopy
OBJDUMP = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-objdump
AR  = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-ar
SIZE = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-size
NM = $(AVR_TOOLS_PATH)$(TOOL_PREFIX)-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

# C compiler
# CC_MAJ:=$(shell $(CC) -dM -E - < /dev/null | grep __GNUC__ | cut -f3 -d\ )
# CC_MIN:=$(shell $(CC) -dM -E - < /dev/null | grep __GNUC_MINOR__ | cut -f3 -d\ )

CC_DUMPVERSION := $(shell $(CC) -dumpversion)
CC_MAJ := $(shell echo $(CC_DUMPVERSION) | cut -d. -f1 -)
CC_MIN := $(shell echo $(CC_DUMPVERSION) | cut -d. -f2 -)
CC_PATCHLEVEL := $(shell echo $(CC_DUMPVERSION) | cut -d. -f3 -)
CC_VER := $(shell expr $(CC_MAJ) \* 10000 + $(CC_MIN) \* 100 + $(CC_PATCHLEVEL))

RELOC_WORKAROUND = 0
F_CPU = 16000000
# 16MHz

IS_MCU = 1

TARGET = $(notdir $(CURDIR))

VPATH = .
VPATH += $(BUILD_DIR)

# Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino

VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/cores/arduino
VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/libraries/SPI/src
VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/libraries/SoftwareSerial/src
VPATH += $(ARDUINO_INSTALL_DIR)/libraries/LiquidCrystal/src
VPATH += $(ARDUINO_INSTALL_DIR)/hardware/arduino/avr/variants/$(HARDWARE_SUB_VARIANT)

LIB_SRC = wiring.c wiring_analog.c wiring_digital.c wiring_shift.c WInterrupts.c hooks.c

LIB_CXXSRC = WMath.cpp WString.cpp Print.cpp SPI.cpp
LIB_CXXSRC += Tone.cpp
LIB_CXXSRC += main.cpp

FORMAT = ihex

# Name of this Makefile (used for "make depend").
MAKEFILE = makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

DEFINES ?=

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)UL
ifneq ($(DEFINES),)
  CDEFS += ${addprefix -D , $(DEFINES)}
endif
CDEFS += -DARDUINO=$(ARDUINO_VERSION)
CXXDEFS = $(CDEFS)

# Add all the source directories as include directories too
CINCS = ${addprefix -I ,${VPATH}}
CXXINCS = ${addprefix -I ,${VPATH}}

# Silence warnings for library code (won't work for .h files, unfortunately)
LIBWARN = -w -Wno-packed-bitfield-compat

# Compiler flag to set the C/CPP Standard level.
CSTANDARD = -std=gnu99
CXXSTANDARD = -std=gnu++11

CDEBUG = -g$(DEBUG)

CWARN = -Wall -Wstrict-prototypes -Wno-packed-bitfield-compat -Wno-pragmas -Wunused-parameter
CXXWARN = -Wall -Wno-packed-bitfield-compat -Wno-pragmas -Wunused-parameter

CTUNING = -fsigned-char -funsigned-bitfields -fno-exceptions
CTUNING += -fshort-enums -ffunction-sections -fdata-sections
CTUNING += -DMOTHERBOARD=${HARDWARE_MOTHERBOARD}

CXXEXTRA = -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti
CFLAGS := $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CEXTRA)   $(CTUNING) $(CSTANDARD)
CXXFLAGS :=         $(CDEFS) $(CINCS) -O$(OPT) $(CXXEXTRA) $(CTUNING) $(CXXSTANDARD)
ASFLAGS :=          $(CDEFS)

LD_PREFIX = -Wl,--gc-sections,--relax
LDFLAGS   = -lm
CTUNING   += -flto

# Programming support using avrdude. Settings and variables.
AVRDUDE_PORT = $(UPLOAD_PORT)
AVRDUDE_WRITE_FLASH = -Uflash:w:$(BUILD_DIR)/$(TARGET).hex:i
AVRDUDE_CONF = $(ARDUINO_INSTALL_DIR)/hardware/tools/avr/etc/avrdude.conf
AVRDUDE_FLAGS = -D -C$(AVRDUDE_CONF) -p$(PROG_MCU) -P$(AVRDUDE_PORT) -c$(AVRDUDE_PROGRAMMER) -b$(UPLOAD_RATE)

# Since Marlin 2.0, the source files may be distributed into several
# different directories, so it is necessary to find them recursively

SRC    = $(shell find src -name '*.c'   -type f)
CXXSRC = $(shell find src -name '*.cpp' -type f)

# Define all object files
OBJ  = ${patsubst %.c, $(BUILD_DIR)/arduino/%.o, ${LIB_SRC}}
OBJ += ${patsubst %.cpp, $(BUILD_DIR)/arduino/%.o, ${LIB_CXXSRC}}
# OBJ += ${patsubst %.S, $(BUILD_DIR)/arduino/%.o, ${LIB_ASRC}}
OBJ += ${patsubst %.c, $(BUILD_DIR)/%.o, ${SRC}}
OBJ += ${patsubst %.cpp, $(BUILD_DIR)/%.o, ${CXXSRC}}

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS   = $(MCU_FLAGS) $(CPU_FLAGS) $(CFLAGS) -I. -Wfatal-errors
ALL_CXXFLAGS = $(MCU_FLAGS) $(CPU_FLAGS) $(CXXFLAGS) -Wfatal-errors

# Defalut target
all: build # echo #build.mkdir #sizeafter

# Remove the content of the build directory
clean:
	@ rm -rf $(BUILD_DIR)/*

# Create required build hierarchy if it does not exist
mkdir:
	@ mkdir -p $(dir $(OBJ))

build: mkdir elf hex bin

elf: $(BUILD_DIR)/$(TARGET).elf

# Link: create ELF output file from library.
$(BUILD_DIR)/$(TARGET).elf: $(OBJ) Configuration.h
	@ echo " CXX  $(notdir $@) <- $(notdir $<)"
	@ $(CXX) $(LD_PREFIX) $(ALL_CXXFLAGS) -o $@ -L. $(OBJ) $(LDFLAGS) $(LD_SUFFIX)

# Object files that were found in "src" will be stored in $(BUILD_DIR)
# in directories that mirror the structure of "src"
$(BUILD_DIR)/%.o: %.c Configuration.h Configuration_adv.h ${MAKEFILE}
	@ echo " CC   $(notdir $@) <- $(notdir $<)"
	@ $(CC) -MMD -c $(ALL_CFLAGS) $(CWARN) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Configuration.h Configuration_adv.h ${MAKEFILE}
	@ echo " CXX  $(notdir $@) <- $(notdir $<)"
	@ $(CXX) -MMD -c $(ALL_CXXFLAGS) $(CXXWARN) $< -o $@

# Object files for Arduino libs will be created in $(BUILD_DIR)/arduino
$(BUILD_DIR)/arduino/%.o: %.c Configuration.h Configuration_adv.h ${MAKEFILE}
	@ echo " CC   $(notdir $@) <- $(notdir $<)"
	@ $(CC) -MMD -c $(ALL_CFLAGS) $(LIBWARN) $< -o $@

$(BUILD_DIR)/arduino/%.o: %.cpp Configuration.h Configuration_adv.h ${MAKEFILE}
	@ echo " CXX  $(notdir $@) <- $(notdir $<)"
	@ $(CXX) -MMD -c $(ALL_CXXFLAGS) $(LIBWARN) $< -o $@

# Automaticaly include the dependency files created by gcc
# This makes sure make sees changes in .h files too
-include ${patsubst %.o, %.d, ${OBJ}}

bin: $(BUILD_DIR)/$(TARGET).bin
hex: $(BUILD_DIR)/$(TARGET).hex
eep: $(BUILD_DIR)/$(TARGET).eep
lss: $(BUILD_DIR)/$(TARGET).lss
sym: $(BUILD_DIR)/$(TARGET).sym

%.hex: %.elf
	@ echo " HEX <- ELF $@"
	@ $(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.bin: %.elf
	@ echo " BIN <- ELF $@"
	@ $(OBJCOPY) -O binary -R .eeprom $< $@

%.eep: %elf
	@ echo " EEP <- ELF $@"
	@ $(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file
%.lss: %.elf
	@ echo " LSS <- ELF $@"
	@ $(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file
%.sym: %.elf
	@ echo " SYM <- ELF $@"
	@ $(NM) -n $< > $@

# Program the device.
# Do not try to reset an Arduino if it's not one
upload: $(BUILD_DIR)/$(TARGET).hex
	@ echo "stty hup < $(UPLOAD_PORT); true"
	@ echo $(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)
	@ echo "stty -hup < $(UPLOAD_PORT); true"
# stty hup < $(UPLOAD_PORT); true
# $(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)
# stty -hup < $(UPLOAD_PORT); true

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(BUILD_DIR)/$(TARGET).hex
ELFSIZE = $(SIZE) $(SIZE_FLAGS) $(BUILD_DIR)/$(TARGET).elf; \
          $(SIZE) $(BUILD_DIR)/$(TARGET).elf


sizebefore:
	@ echo "make:sizebefore"
	@ if [ -f $(BUILD_DIR)/$(TARGET).elf ]; then \
	echo $(MSG_SIZE_BEFORE); $(HEXSIZE); \
	else \
	echo "$(BUILD_DIR)/$(TARGET).elf not found"; \
	fi;

sizeafter: build
	@ echo "make:sizeafter"
	@ if [ -f $(BUILD_DIR)/$(TARGET).elf ]; then \
	echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
	else \
	echo "$(BUILD_DIR)/$(TARGET).elf not found"; \
	fi;

.SUFFIXES:

echo: ${MAKEFILE}
	@echo "Bare Marlin Firmware Arduino Project Makefile"
	@echo "Echo of the internal state"
	@echo -----
	@echo "OS : $(OS)"
	@echo "ARDUINO_INSTALL_DIR : $(ARDUINO_INSTALL_DIR)"
	@echo "ARDUINO_USER_DIR : $(ARDUINO_USER_DIR)"
	@echo "AVR_TOOLS_PATH : $(AVR_TOOLS_PATH)"
	@echo "HARDWARE_MOTHERBOARD : $(HARDWARE_MOTHERBOARD)"
	@echo -----
	@echo "ARDUINO_DUMPVERSION : $(ARDUINO_DUMPVERSION)"
	@echo "ARDUINO_MAJ : $(ARDUINO_MAJ)"
	@echo "ARDUINO_MIN : $(ARDUINO_MIN)"
	@echo "ARDUINO_PATCHLEVEL : $(ARDUINO_PATCHLEVEL)"
	@echo -----
	@echo "HARDWARE_VARIANT : $(HARDWARE_VARIANT)"
	@echo "HARDWARE_SUB_VARIANT : $(HARDWARE_SUB_VARIANT)"
	@echo "MCU : $(MCU)"
	@echo "PROG_MCU : $(PROG_MCU)"
	@echo "TOOL_PREFIX : $(TOOL_PREFIX)"
	@echo "MCU_FLAGS : $(MCU_FLAGS)"
	@echo "SIZE_FLAGS : $(SIZE_FLAGS)"
	@echo -----
	@echo "UPLOAD_RATE : $(UPLOAD_RATE)"
	@echo "AVRDUDE_PROGRAMMER : $(AVRDUDE_PROGRAMMER)"
	@echo "UPLOAD_PORT : $(UPLOAD_PORT)"
	@echo "BUILD_DIR : $(BUILD_DIR)"
	@echo "UPLOAD_PORT : $(UPLOAD_PORT)"
	@echo -----
	@echo "LIQUID_TWI2 : $(LIQUID_TWI2)"
	@echo "WIRE : $(WIRE)"
	@echo "TONE : $(TONE)"
	@echo "U8GLIB : $(U8GLIB)"
	@echo "TMC : $(TMC)"
	@echo "NEOPIXEL : $(NEOPIXEL)"
	@echo -----
	@echo "CC : $(CC)"
	@echo "CXX : $(CXX)"
	@echo "OBJCOPY : $(OBJCOPY)"
	@echo "OBJDUMP : $(OBJDUMP)"
	@echo "AR : $(AR)"
	@echo "SIZE : $(SIZE)"
	@echo "NM : $(NM)"
	@echo "AVRDUDE : $(AVRDUDE)"
	@echo "REMOVE : $(REMOVE)"
	@echo "MV : $(MV)"
	@echo -----
	@echo "CC_DUMPVERSION : $(CC_DUMPVERSION)"
	@echo "CC_MAJ : $(CC_MAJ)"
	@echo "CC_MIN : $(CC_MIN)"
	@echo "CC_PATCHLEVEL : $(CC_PATCHLEVEL)"
	@echo -----
	@echo "RELOC_WORKAROUND : $(RELOC_WORKAROUND)"
	@echo "F_CPU : $(F_CPU)"
	@echo "CURDIR : $(CURDIR)"
	@echo "TARGET : $(TARGET)"
	@echo "VPATH : $(VPATH)"
	@echo -----
	@echo "LIB_SRC : $(LIB_SRC)"
	@echo "LIB_CXXSRC : $(LIB_CXXSRC)"
	@echo -----
	@echo "FORMAT : $(FORMAT)"
	@echo "MAKEFILE : $(MAKEFILE)"
	@echo "DEBUG : $(DEBUG)"
	@echo "OPT : $(OPT)"
	@echo "DEFINES : $(DEFINES)"
	@echo "CDEFS : $(CDEFS)"
	@echo "CXXDEFS : $(CXXDEFS)"
	@echo "CINCS : $(CINCS)"
	@echo "CXXINCS : $(CXXINCS)"
	@echo "LIBWARN : $(LIBWARN)"
	@echo "CSTANDARD : $(CSTANDARD)"
	@echo "CXXSTANDARD : $(CXXSTANDARD)"
	@echo "CDEBUG : $(CDEBUG)"
	@echo "CWARN : $(CWARN)"
	@echo "CXXWARN : $(CXXWARN)"
	@echo "CTUNING : $(CTUNING)"
	@echo -----
	@echo "CXXEXTRA : $(CXXEXTRA)"
	@echo "CFLAGS : $(CFLAGS)"
	@echo "CXXFLAGS : $(CXXFLAGS)"
	@echo "ASFLAGS : $(ASFLAGS)"
	@echo "LD_PREFIX : $(LD_PREFIX)"
	@echo "LDFLAGS : $(LDFLAGS)"
	@echo "CTUNING : $(CTUNING)"
	@echo "AVRDUDE_PORT : $(AVRDUDE_PORT)"
	@echo "AVRDUDE_WRITE_FLASH : $(AVRDUDE_WRITE_FLASH)"
	@echo "AVRDUDE_CONF : $(AVRDUDE_CONF)"
	@echo "AVRDUDE_FLAGS : $(AVRDUDE_FLAGS)"
	@echo "SRC : $(SRC)"
	@echo "CXXSRC : $(CXXSRC)"
	@echo "OBJ : $(OBJ)"
	@echo "LST : $(LST)"
	@echo "ALL_CFLAGS : $(ALL_CFLAGS)"
	@echo "ALL_CXXFLAGS : $(ALL_CXXFLAGS)"
	@echo "ALL_ASFLAGS : $(ALL_ASFLAGS)"