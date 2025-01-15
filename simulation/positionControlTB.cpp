#include <cstdlib>
#include <cstring>
#include <vector>
#include "../include/positionControl.h"

#define DATA_SIZE 1000

extern "C" {
    // Fonction pour allouer un tableau dynamique depuis un std::vector
    int get_values(int** data, int** time) {
        std::vector<int> vecData;
        std::vector<int> vecTime;

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
            vecData.push_back(i);
            vecTime.push_back(i);
            //vecData.push_back(current);
            previous = current;
            previousspeed = currentspeed;
        }

        int size = vecData.size(); // Taille du tableau

        // Allouer de la mémoire dynamique
        *data = (int*)malloc(size * sizeof(int));
        *time = (int*)malloc(size * sizeof(int));

        // Copier les données du std::vector dans le tableau alloué
        std::memcpy(*data, vecData.data(), size * sizeof(int));
        std::memcpy(*time, vecTime.data(), size * sizeof(int));

        return size;
    }

    // Fonction pour libérer la mémoire allouée dynamiquement
    void free_values(int* vecData,int* vecTime) {
        free(vecData);
        free(vecTime);
    }
}
