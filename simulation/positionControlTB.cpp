#include <cstdlib>
#include <cstring>
#include <vector>
#include "../include/positionControl.h"

#define DATA_SIZE 500

extern "C" {
    // Fonction pour allouer un tableau dynamique depuis un std::vector
    int get_values(double** data, int** time) {
        std::vector<double> vecData;
        std::vector<int> vecTime;

        positionControl positionControlLineaire;
        positionControlLineaire.vitesseMaxAv.setMaxValue(365);
        positionControlLineaire.accelerationMaxAv.setMaxValue(300);
        positionControlLineaire.decelerationMaxAv.setMaxValue(300);
        positionControlLineaire.vitesseMaxAr.setMaxValue(365);
        positionControlLineaire.accelerationMaxAr.setMaxValue(300);
        positionControlLineaire.decelerationMaxAr.setMaxValue(300);
        positionControlLineaire.decelationLineair = true;
        positionControlLineaire.decelerationStop = 600;
        positionControlLineaire.setConsigne(1000);

        int previous = 0;
        double current;
        int previousspeed = 0;
        int currentspeed = 0;
        // Initialisation de vos données (exemple aléatoire)
        for (int i = 0; i < DATA_SIZE; i+=1) {
            current = positionControlLineaire.getPostion(i*20);
            currentspeed = (previous - current);
            vecData.push_back(current);
            vecTime.push_back(i*20);
            //vecData.push_back(current);
            previous = current;
            previousspeed = currentspeed;
        }

        int size = vecData.size(); // Taille du tableau

        // Allouer de la mémoire dynamique
        *data = (double*)malloc(size * sizeof(double));
        *time = (int*)malloc(size * sizeof(int));

        // Copier les données du std::vector dans le tableau alloué
        std::memcpy(*data, vecData.data(), size * sizeof(double));
        std::memcpy(*time, vecTime.data(), size * sizeof(int));

        return size;
    }

    // Fonction pour libérer la mémoire allouée dynamiquement
    void free_values(double* vecData,int* vecTime) {
        free(vecData);
        free(vecTime);
    }
}
