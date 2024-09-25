#ifndef RENDERING_H
#define RENDERING_H

#include "main.h"

void drawMiniMap(SDL_Renderer* renderer, double posX, double posY, double dirX, double dirY);
void drawRain(SDL_Renderer* renderer);
void performRaycasting(SDL_Renderer* renderer, TextureData wallTextures[], TextureData* skyTexture, double posX, double posY, double dirX, double dirY, double planeX, double planeY);

#endif
