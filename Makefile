CXX		  := g++
CXX_FLAGS := -Wall -Wextra -O2 -std=c++17 

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)
run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

test: clean all
	python 
clean:
	-rm $(BIN)/*
