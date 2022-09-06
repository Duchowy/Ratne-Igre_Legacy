SRC = main.cpp level.cpp load_level.cpp jet.cpp movement.cpp select_level.cpp render_level.cpp
DEPS = $(addprefix src/,$(SRC))
INCL = main.h level.h load_level.h jet.h movement.h common.h enum.h select_level.h render_level.h
LIB = -Wl,-Bdynamic -fopenmp -lallegro -lallegro_font -lallegro_image -lallegro_primitives -lallegro_ttf -lallegro_audio -lallegro_acodec -lconfig++ -Wl,-Bstatic -lstdc++ -lpthread
LIB_PATH = -I ./deps_alleg/include -L ./deps_alleg/lib
OBJ = $(SRC:.cpp=.o)
EXE = debug/ratne.exe
RELEASE_EXE = release/ratne.exe

CC = g++
CFLAGS = -std=c++11 -Wall
LDFLAGS = -o $(EXE)
CFDEBUG = -g -D DEBUG
RELEASE_CFLAGS = -O2 -s -mwindows -static-libgcc -static-libstdc++ 

$(EXE): $(DEPS)
	g++ -o $@ $^ $(CFLAGS) $(CFDEBUG) $(LIB_PATH) $(LIB)

distr:
	g++ -o $(RELEASE_EXE) $(DEPS) $(CFLAGS) $(RELEASE_CFLAGS) $(LIB_PATH) $(LIB)