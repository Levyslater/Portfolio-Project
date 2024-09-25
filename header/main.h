#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "constants.h"
#include "raindrops.h"
#include <stdbool.h>
#include "map.h"

#define MINIMAP_SCALE 5

extern Mix_Chunk* shotgunSound;

typedef enum {
    WEAPON_AIM,
    WEAPON_FIRE,
    WEAPON_RECOIL,
    WEAPON_RELOAD
} WeaponState;

typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} TextureData;


void initializeRaindrops();
void handlePlayerInput(SDL_Event* event, bool* running, bool* showMiniMap, bool* showRain, double* posX, double* posY, double* dirX, double* dirY, double* planeX, double* planeY);

#endif
