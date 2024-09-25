#include "../header/textures.h"

/**
 * loadTexture - function to load textures
 * @path: pointer to the texture path
 * @renderer: SDL renderer pointer
 * 
 * Return: pointer to the texture
 */
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(path);

    if (!surface)
    {
        fprintf(stderr, "IMG_Load error: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}


/**
 * loadTextureWithDimensions - function to load textures and get their dimensions
 * @file: path to the texture file
 * @renderer: SDL renderer pointer
 * 
 * Return: pointer to the texture
 */
TextureData loadTextureWithDimensions(const char* file, SDL_Renderer* renderer)
{
    /*Load texture and get dimensions*/
    TextureData texData;
    texData.texture = loadTexture(file, renderer);
    SDL_QueryTexture(texData.texture, NULL, NULL, &texData.width, &texData.height);
    return texData;
}
