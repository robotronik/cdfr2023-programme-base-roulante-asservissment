#include <cstdlib>
#include <cstring>
#include <vector>
#include "../include/positionControl.h"

#define DATA_SIZE 1000

extern "C" {
    // Fonction pour allouer un tableau dynamique depuis un std::vector
    int* get_values(int* size) {


        std::vector<int> values;

        positionControl positionControlLineaire;
        positionControlLineaire.vitesseMaxAv =  500;
        positionControlLineaire.accelerationMaxAv = 250; 
        positionControlLineaire.decelerationMaxAv = 250;
        positionControlLineaire.vitesseMaxAr = 500;
        positionControlLineaire.accelerationMaxAr = 250; 
        positionControlLineaire.decelerationMaxAr = 250;
        positionControlLineaire.setConsigne(1000);
        positionControlLineaire.decelationLineair = false;

        int previous = 0;
        int current;
        int previousspeed = 0;
        int currentspeed = 0;
        // Initialisation de vos données (exemple aléatoire)
        for (int i = 0; i < DATA_SIZE; ++i) {
            current = positionControlLineaire.getPostion(i*10)*1000;
            currentspeed = (previous - current);
            values.push_back(i);
            //values.push_back(current);
            previous = current;
            previousspeed = currentspeed;
        }

        //static const std::vector<int> values = {1, 2, 3, 4, 5};
        *size = values.size(); // Taille du tableau

        // Allouer de la mémoire dynamique
        int* result = (int*)malloc(*size * sizeof(int));

        // Copier les données du std::vector dans le tableau alloué
        std::memcpy(result, values.data(), *size * sizeof(int));

        return result;
    }

    // Fonction pour libérer la mémoire allouée dynamiquement
    void free_values(int* values) {
        free(values);
    }
}
