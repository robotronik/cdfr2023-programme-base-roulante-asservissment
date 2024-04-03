#include <SDL2/SDL.h>
#include "../include/positionControl.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define DATA_SIZE 1000

int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;
    int data[DATA_SIZE]; // Votre variable

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Création de la fenêtre
    window = SDL_CreateWindow("Evolution de la variable", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Création du renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    positionControl positionControlLineaire;
    positionControlLineaire.vitesseMaxAv =  500;
    positionControlLineaire.accelerationMaxAv = 250; 
    positionControlLineaire.decelerationMaxAv = 250;
    positionControlLineaire.vitesseMaxAr = 500;
    positionControlLineaire.accelerationMaxAr = 250; 
    positionControlLineaire.decelerationMaxAr = 250;
    positionControlLineaire.setConsigne(-1000,0);
    positionControlLineaire.decelationLineair = false;

    int previous = 0;
    int current;
    int previousspeed = 0;
    int currentspeed = 0;
    // Initialisation de vos données (exemple aléatoire)
    for (int i = 0; i < DATA_SIZE; ++i) {
        current = positionControlLineaire.getPostion(i*10)*1000;
        currentspeed = (previous - current);
        printf("%d\n",(previousspeed - currentspeed)*20);
        data[i] = (previousspeed - currentspeed)*20 + 4000 ;currentspeed; 
        previous = current;
        previousspeed = currentspeed;
    }

    // Boucle principale
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Effacement de l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dessiner les courbes
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 1; i < DATA_SIZE; ++i) {
            SDL_RenderDrawLine(renderer, i - 1, SCREEN_HEIGHT - (data[i - 1]/10), i, SCREEN_HEIGHT - (data[i]/10));
        }

        // Rafraîchissement de l'affichage
        SDL_RenderPresent(renderer);
    }

    // Libération des ressources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}