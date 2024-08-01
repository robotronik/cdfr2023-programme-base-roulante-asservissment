#pragma once
#include <iostream>
#include <vector>
#include <cstdint>

class DataPacker {
private:
    std::vector<uint8_t> data;

public:

    // Constructeur
    DataPacker(){}

    // Ajoute un uint8_t dans le tableau
    void addUint8(uint8_t value) {
        data.push_back(value);
    }

    // Ajoute un uint16_t dans le tableau, en le divisant en deux uint8_t
    void addUint16(uint16_t value) {
        data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF)); // Partie haute
        data.push_back(static_cast<uint8_t>(value & 0xFF));        // Partie basse
    }

    // Ajoute un tableau de uint8_t au DataPacker
    void addArray(const uint8_t* array, size_t length) {
        data.insert(data.end(), array, array + length);
    }

    // Retourne un pointeur vers le tableau de uint8_t
    uint8_t* getData() {
        return data.data();
    }

    // Retourne la taille du tableau
    size_t getSize() const {
        return data.size();
    }

    // Affiche les données pour le débogage
    void printData() const {
        for (auto val : data) {
            std::cout << static_cast<int>(val) << " ";
        }
        std::cout << std::endl;
    }
};

