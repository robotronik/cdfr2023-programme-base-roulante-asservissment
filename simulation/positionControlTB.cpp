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

        SET_TIME_(0);

        positionControl positionControlLineaire;
        positionControlLineaire.vitesseMaxAv.setRange(0,700);
        positionControlLineaire.accelerationMaxAv.setRange(0,600);
        positionControlLineaire.decelerationMaxAv.setRange(0,600);
        positionControlLineaire.vitesseMaxAr.setRange(0,700);
        positionControlLineaire.accelerationMaxAr.setRange(0,600);
        positionControlLineaire.decelerationMaxAr.setRange(0,600);
        positionControlLineaire.decelerationStopAv.setRange(0,1200);
        positionControlLineaire.decelerationStopAr.setRange(0,1200);
        positionControlLineaire.vitesseMaxAv.setMax();
        positionControlLineaire.accelerationMaxAv.setMax();
        positionControlLineaire.decelerationMaxAv.setMax();
        positionControlLineaire.vitesseMaxAr.setMax();
        positionControlLineaire.accelerationMaxAr.setMax();
        positionControlLineaire.decelerationMaxAr.setMax();
        positionControlLineaire.decelerationStopAv.setMax();
        positionControlLineaire.decelerationStopAr.setMax();

        positionControlLineaire.setPosition(300);
        positionControlLineaire.setConsigne(0);
        positionControlLineaire.setMaxSpeedOut(700);

        double current;
        int numCommande = 0;
        // Initialisation de vos données (exemple aléatoire)
        for (int i = 0; i < DATA_SIZE; i+=1) {
            SET_TIME_(i*20);
            current = positionControlLineaire.getPostion();
            vecData.push_back(current);
            vecTime.push_back(i*20);
            if(i>10 && numCommande == 0 && positionControlLineaire.getPostion()==0){
                positionControlLineaire.stop();
                positionControlLineaire.setPosition(300);
                positionControlLineaire.setConsigne(0);
                positionControlLineaire.setMaxSpeedOut(0);
                numCommande ++;
            }
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
