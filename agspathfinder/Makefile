UNAME := $(shell uname)

INCDIR = ../../ags/Engine ../../ags/Common
LIBDIR =
CC = gcc
CXX = g++
CFLAGS = -fPIC -fvisibility=hidden -O2 -g -Wall -std=c++11
LIBS = -lm -lstdc++

ifeq ($(UNAME), Darwin)
TARGET = libagspathfinder.dylib
CFLAGS += -DMAC_VERSION
else
TARGET = libagspathfinder.so
CFLAGS += -DLINUX_VERSION
endif

CXXFLAGS = $(CFLAGS)

include Makefile-objs


CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)


.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) -shared -dynamiclib -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

%.o: %.c
	@echo $@
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	@echo $@
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@rm -f $(TARGET) $(OBJS)
