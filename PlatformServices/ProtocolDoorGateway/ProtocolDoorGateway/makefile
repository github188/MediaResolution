ver=release

GXX=g++
GCC=gcc

ifeq ($(ver), release)
GXX_FLAGS=-Wall
GCC_FLAGS=-Wall
VERSION=Release
else
GXX_FLAGS=-g -Wall
GCC_FLAGS=-g -Wall
VERSION=Debug
endif

CUR_PATH = $(shell pwd)

StandaryLibrary_PATH = ../StandaryLibrary

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(StandaryLibrary_PATH)/lib:$(StandaryLibrary_PATH)/poco-linux-1.4.6p2/lib

GXX_LIBS_PATH=-L$(CUR_PATH)/lib \
              -L$(StandaryLibrary_PATH)/poco-linux-1.4.6p2/lib \
              -L$(StandaryLibrary_PATH)/lib

GXX_INCLUDES=-I$(CUR_PATH) \
						 -I$(StandaryLibrary_PATH)/poco-linux-1.4.6p2/Net/include \
             -I$(StandaryLibrary_PATH)/poco-linux-1.4.6p2/Foundation/include 


LIBS=-lPocoNet -lPocoFoundation


TARGET=NewAccessServer.a
BIN=NewAccessServer

ALL_SOURCE_CPP=$(shell find ./ -name '*.cpp')
ALL_SOURCE_OBJS=$(ALL_SOURCE_CPP:%.cpp=%.o)

$(TARGET):$(ALL_SOURCE_OBJS)
	$(GXX) -o $@ $^ $(GXX_LIBS_PATH) $(LIBS) 
	mv -f $(TARGET) ./bin/$(VERSION)/$(BIN)
	rm -rf $(ALL_SOURCE_OBJS)
%.o:%.cpp
	$(GXX) -o $@ -c $(GXX_FLAGS) $< $(GXX_INCLUDES)

.PHONY:clean
clean:
	rm -rf $(TARGET) $(ALL_SOURCE_OBJS)
