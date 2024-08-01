#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>

class DataUnpacker {
private:
    std::vector<uint8_t> data;
    size_t currentIndex; // Pour suivre la position actuelle lors du pop

public:
    // Constructeur prenant un tableau et sa taille
    DataUnpacker(const uint8_t* array, size_t length) : data(array, array + length), currentIndex(0) {}

    // Extrait et retourne un uint8_t du début du tableau
    uint8_t popUint8() {
        if (currentIndex >= data.size()) {
            throw std::out_of_range("No more data to pop");
        }
        return data[currentIndex++];
    }

    // Extrait et retourne un uint16_t du début du tableau
    uint16_t popUint16() {
        if (currentIndex + 1 >= data.size()) {
            throw std::out_of_range("No more data to pop");
        }
        uint16_t value = static_cast<uint16_t>(data[currentIndex]) |
                         (static_cast<uint16_t>(data[currentIndex + 1]) << 8);
        currentIndex += 2;
        return value;
    }

    // Affiche les données restantes pour le débogage
    void printData() const {
        for (size_t i = currentIndex; i < data.size(); ++i) {
            std::cout << static_cast<int>(data[i]) << " ";
        }
        std::cout << std::endl;
    }
};
