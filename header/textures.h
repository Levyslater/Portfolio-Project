#ifndef TEXTURES_H
#define TEXTURES_H

#include "main.h"

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
TextureData loadTextureWithDimensions(const char* file, SDL_Renderer* renderer);

#endif
