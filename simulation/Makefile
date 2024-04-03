# Compiler
CC = g++

# Options de compilation
CFLAGS = -Wall -Wextra -std=c++11
LIBS = -lm $(shell sdl2-config --cflags --libs)
INCLUDES = ../include/positionControl.h

# Fichiers source
SOURCES = main.c 
SOURCES += ../src/positionControl.cpp

# Nom de l'exécutable
EXECUTABLE = simulation

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(EXECUTABLE) $(LIBS)

clean:
	rm -f $(EXECUTABLE)
