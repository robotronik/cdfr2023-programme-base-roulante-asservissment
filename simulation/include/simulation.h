#pragma once
#include <gtk/gtk.h>
#include "commands.h"
#include <iostream>
#include <vector>
#include <string>

#define FUNCTIONSIM(funcName)                                       \
    void funcName(void);                                           \
    struct Register_##funcName {                                   \
        Register_##funcName() {                                    \
            functionVector.emplace_back(funcName, #funcName);      \
        }                                                          \
    } register_##funcName;                                         \
    void funcName(void)

class FunctionContainer {
public:
    // Type de fonction
    using FunctionType = void(*)(void);

    // Constructeur
    FunctionContainer(FunctionType func, const std::string& name)
        : func_(func), name_(name) {}

    // Appeler la fonction
    void call() const {
        if (func_) func_();
    }

    // Obtenir le nom de la fonction
    std::string getName() const {
        return name_;
    }

private:
    FunctionType func_;
    std::string name_;
};

// Vecteur global pour stocker les fonctions
extern std::vector<FunctionContainer> functionVector;

gpointer simulationProcess(gpointer data);
