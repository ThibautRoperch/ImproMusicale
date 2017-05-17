CFLAGS=-Wall -std=c++11
OFLAGS=-O2

BUILD_DIR = ./build
SRC_DIR = ./src

all: $(BUILD_DIR)/epurer_musicxml.exe $(BUILD_DIR)/main.exe $(BUILD_DIR)/validation_melodie_generee.exe

$(BUILD_DIR)/epurer_musicxml.exe: $(SRC_DIR)/epurer_musicxml.cpp
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

$(BUILD_DIR)/main.exe: $(SRC_DIR)/main.cpp $(SRC_DIR)/note.cpp
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

$(BUILD_DIR)/validation_melodie_generee.exe: $(SRC_DIR)/validation_melodie_generee.cpp $(SRC_DIR)/note.cpp
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

epurer:
	./$(BUILD_DIR)/epurer_musicxml.exe ${in} ${out}

modeliser:
	./$(BUILD_DIR)/main.exe ${in} ${out}

verifier:
	./$(BUILD_DIR)/validation_melodie_generee.exe ${in} ${genere}

clean:
	rm -f $(BUILD_DIR)/*
