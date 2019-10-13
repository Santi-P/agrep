CXX		  := g++
CXX_FLAGS :=  -O2 -std=c++17 -lboost_program_options 

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib
DOCS	:= docs

LIBRARIES	:=
EXECUTABLE	:= agrep


all: $(BIN)/$(EXECUTABLE)
run: clean all 
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

test: clean all
	- bash tests/test.sh;

clean:
	-rm $(BIN)/*
	-rm -r $(DOCS)/html

document: 
	-doxygen doxyfile.mk

