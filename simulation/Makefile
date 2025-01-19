
# Variables
CXX = g++
CXXFLAGS = -Wall -g -O0 -shared -fPIC -Wextra -std=c++17 -DSIMULATION_POSITION_CONTROL
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
INCLUDES = -I../include
SRC_DIR = ../src
TARGET = positionControlTB.so

# Fichiers nécessaires
SOURCES = $(SRC_DIR)/positionControl.cpp positionControlTB.cpp

# Règle par défaut
all: $(TARGET)

# Compilation directe
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

# Nettoyage
clean:
	rm -f $(TARGET)

# PHONY pour éviter des conflits avec des fichiers du même nom
.PHONY: all clean

