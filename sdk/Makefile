HOME_TREE := ../

MODULE_NAME := $(notdir $(CURDIR))

include $(HOME_TREE)/mak_def.inc

CXXSRC += src/rplidar_driver.cpp \
          src/hal/thread.cpp

C_INCLUDES += -I$(CURDIR)/include -I$(CURDIR)/src

ifeq ($(BUILD_TARGET_PLATFORM),Linux)
CXXSRC += src/arch/linux/net_serial.cpp \
          src/arch/linux/net_socket.cpp \
          src/arch/linux/timer.cpp 
endif


ifeq ($(BUILD_TARGET_PLATFORM),Darwin)
CXXSRC += src/arch/macOS/net_serial.cpp \
          src/arch/macOS/net_socket.cpp \
          src/arch/macOS/timer.cpp
endif

all: build_sdk

include $(HOME_TREE)/mak_common.inc

clean: clean_sdk
