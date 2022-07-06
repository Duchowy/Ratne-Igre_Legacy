SRC = main.cpp level.cpp load_level.cpp jet.cpp movement.cpp
DEPS = $(addprefix src/,$(SRC))
INCL = main.h level.h load_level.h jet.h movement.h common.h enum.h
LIB = -lallegro -lallegro_font -lallegro_image -lallegro_primitives -fopenmp
LIB_PATH = -I deps/include -L deps/lib
OBJ = $(SRC:.cpp=.o)
EXE = debug/ratne.exe
RELEASE_EXE = release/ratne.exe

CC = g++
CFLAGS = -std=c++11 -Wall
LDFLAGS = -o $(EXE)
CFDEBUG = -g
RELEASE_CFLAGS = -O2 -s -mwindows -static-libgcc -static-libstdc++

$(EXE): $(DEPS)
	g++ -o $@ $^ $(CFLAGS) $(CFDEBUG) $(LIB_PATH) $(LIB)

distr:
	g++ -o $(RELEASE_EXE) $(DEPS) $(CFLAGS) $(RELEASE_CFLAGS) $(LIB_PATH) $(LIB)