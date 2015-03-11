EXECUTABLE=AB08XX
BUILD_DIR=build
MAIN_DIR=../..
CC=g++
CFLAGS=-c -Wall -g -I$(MAIN_DIR) -I. -I..
LDFLAGS=-g
VPATH=$(MAIN_DIR):$(MAIN_DIR)/examples
SOURCES=AB08XX.cpp AB08XX_SPI_Linux.cpp  main.cpp AB08XX_CLI.cpp Arduino.cpp Time.cpp HardwareSerial.cpp AString.cpp
OBJECTS=$(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))

all: $(SOURCES) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)	

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(BUILD_DIR)/*.o -o $@

$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm $(BUILD_DIR)/*.o
	rmdir $(BUILD_DIR)
	rm $(EXECUTABLE)
