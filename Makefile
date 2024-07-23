TARGET = bin/simulationAsserv
SRCDIR = src simulation/src
INCLUDE_DIR = include 
INCLUDE_SIM = simulation/include
BINDIR = bin
OBJDIR = obj


SRC = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ = $(SRC:$(SRCDIR)/%.c=$(BINDIR)/%.o)

CFLAGS = -std=c++17 -Wall -Wno-unused-parameter -g -O0 -I$(INCLUDE_SIM) -I$(INCLUDE_DIR) -Wextra -O2 -DSIMULATION
LDLIBS = -pthread

CXX = g++



all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJ) | $(BINDIR)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDLIBS)



# Compilation des fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CFLAGS) -MMD -MP -c $< -o $@

# Création directori
$(OBJDIR):
	mkdir -p $@

# Création directori
$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)


.PHONY: all clean $(TARGET)
