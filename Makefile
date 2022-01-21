OBJECTS := $(shell find . -type f -iname "*.cc")
CCFLAGS := -g -fexpensive-optimizations -flto -Ofast -Wunreachable-code -Wno-deprecated -Wno-unknown-pragmas -DUNICODE -L. -lm -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
EXECUTABLE_PATH := ./

all:
	@g++ -o $(EXECUTABLE_PATH)a.out $(OBJECTS) $(CCFLAGS)