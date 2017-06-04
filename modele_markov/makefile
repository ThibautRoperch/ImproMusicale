CFLAGS=-Wall -std=c++11
OFLAGS=-O2

SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin
LIB_DIR = ./lib

LibMidifile = $(LIB_DIR)/craigsapp-midifile-5a7dc97

PrgmMoc = /usr/lib/x86_64-linux-gnu/qt5/bin/moc

all: initialiser compiler_sources

initialiser:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

compiler_librairies:
	cd $(LibMidifile)
	make

compiler_sources: $(BIN_DIR)/epurer_musicxml.exe $(BIN_DIR)/epurer_midi.exe $(BIN_DIR)/modeliser.exe $(BIN_DIR)/verifier_melodie_generee.exe $(BIN_DIR)/convertir_musicxml.exe

$(BIN_DIR)/epurer_musicxml.exe: $(SRC_DIR)/epurer_musicxml.cpp
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

$(BIN_DIR)/epurer_midi.exe: $(SRC_DIR)/epurer_midi.cpp $(LibMidifile)/obj/*
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

$(BIN_DIR)/modeliser.exe: $(SRC_DIR)/modeliser.cpp $(SRC_DIR)/note.cpp
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

$(BIN_DIR)/verifier_melodie_generee.exe: $(SRC_DIR)/verifier_melodie_generee.cpp $(SRC_DIR)/note.cpp
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

$(BIN_DIR)/convertir_musicxml.exe: $(SRC_DIR)/convertir_musicxml.cpp
	g++ -o $@ $^ $(CFLAGS) $(OFLAGS)

epurerXML:
	./$(BIN_DIR)/epurer_musicxml.exe ${in} ${out}

epurerMIDI:
	./$(BIN_DIR)/epurer_midi.exe ${in} ${out}

modeliser:
	./$(BIN_DIR)/modeliser.exe ${in} ${out}

verifier:
	./$(BIN_DIR)/verifier_melodie_generee.exe ${in} ${genere}

convertir:
	./$(BIN_DIR)/convertir_musicxml.exe ${in} ${out}

launch:
	./$(BIN_DIR)/main.exe

clean_librairies:
	cd $(LibMidifile)
	make clean

clean:
	rm -f $(BUILD_DIR)/*
	rm -f $(BIN_DIR)/*
