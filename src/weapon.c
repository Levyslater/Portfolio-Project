#include <stdbool.h>
#include "../header/weapon.h"


/**
 * handleWeaponInput - Function handles player weapon input
 * @firing: pointer to a boolean to control weapon firing
 * @currentWeaponState: current weapon state
 * 
 * Return: weapon state
 */
WeaponState handleWeaponInput(bool* firing, WeaponState currentWeaponState) {
    WeaponState newState = currentWeaponState;

    /*Check for spacebar press to fire weapon*/
    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_SPACE] && !(*firing))
    {
        *firing = true;
        newState = WEAPON_FIRE; // Set weapon to fire state
    }

    /*Handle state transitions*/
    switch (currentWeaponState)
    {
        case WEAPON_AIM:
            if (*firing) newState = WEAPON_FIRE;
            break;
        case WEAPON_FIRE:
            newState = WEAPON_RECOIL;
            break;
        case WEAPON_RECOIL:
            newState = WEAPON_RELOAD;
            break;
        case WEAPON_RELOAD:
        /*Allow firing again after reload*/
            *firing = false;
            newState = WEAPON_AIM;
            break;
    }

    return newState;
}

/**
 * renderWeapon - function to render the weapon textures
 * @renderer: SDL renderer pointer to render weapon textures
 * @weaponTextures: pointer to textures
 * weaponState: struct to weapon states
 * 
 * Return: void
 */
void renderWeapon(SDL_Renderer* renderer, TextureData* weaponTextures, WeaponState weaponState)
{
    /*Adjust Weapon size*/
    SDL_Rect dstRect = {SCREEN_WIDTH / 2 - 128, SCREEN_HEIGHT - 256, 256, 256};

    switch (weaponState)
    {
        case WEAPON_AIM:
            SDL_RenderCopy(renderer, weaponTextures[0].texture, NULL, &dstRect);
            break;
        case WEAPON_FIRE:
            SDL_RenderCopy(renderer, weaponTextures[1].texture, NULL, &dstRect);
            break;
        case WEAPON_RECOIL:
            SDL_RenderCopy(renderer, weaponTextures[2].texture, NULL, &dstRect);
            break;
        case WEAPON_RELOAD:
            SDL_RenderCopy(renderer, weaponTextures[3].texture, NULL, &dstRect);
            break;
    }
}

/**
 * updateWeaponState - function to update weapon state
 * @lastFireTime: pointer to time data when weapon was fired last
 * @fireCooldown: contains the time in ms for weapon cooldown
 * 
 * Return: null
 */
void updateWeaponState(WeaponState* weaponState, Uint32* lastFireTime, Uint32 fireCooldown)
{
    /*Update weapon state based on current time and last fire time*/
    Uint32 currentTime = SDL_GetTicks();

    /*Handle weapon state transitions*/
    switch (*weaponState)
    {
        case WEAPON_FIRE:

            if (currentTime - *lastFireTime >= fireCooldown)
            {
                *weaponState = WEAPON_RECOIL;
            }
            break;
        case WEAPON_RECOIL:
            *weaponState = WEAPON_RELOAD;
            break;
        case WEAPON_RELOAD:
            *weaponState = WEAPON_AIM;
            break;
        default:
            *weaponState = WEAPON_AIM;
            break;
    }
}
