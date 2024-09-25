#include "../header/textures.h"

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        fprintf(stderr, "IMG_Load error: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

TextureData loadTextureWithDimensions(const char* file, SDL_Renderer* renderer) {
    TextureData texData;
    texData.texture = loadTexture(file, renderer);
    SDL_QueryTexture(texData.texture, NULL, NULL, &texData.width, &texData.height);
    return texData;
}
