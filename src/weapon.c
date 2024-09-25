#include <stdbool.h>
#include "../header/weapon.h"

WeaponState handleWeaponInput(bool* firing, WeaponState currentWeaponState) {
    WeaponState newState = currentWeaponState;

    // Check for spacebar press to fire weapon
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_SPACE] && !(*firing)) {
        *firing = true;
        newState = WEAPON_FIRE; // Set weapon to fire state
    }

    // Handle state transitions
    switch (currentWeaponState) {
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
            *firing = false; // Allow firing again after reload
            newState = WEAPON_AIM;
            break;
    }

    return newState;
}
void renderWeapon(SDL_Renderer* renderer, TextureData* weaponTextures, WeaponState weaponState) {
    SDL_Rect dstRect = {SCREEN_WIDTH / 2 - 128, SCREEN_HEIGHT - 256, 256, 256}; // Adjust weapon size

    switch (weaponState) {
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

void updateWeaponState(WeaponState* weaponState, Uint32* lastFireTime, Uint32 fireCooldown) {
    Uint32 currentTime = SDL_GetTicks();

    switch (*weaponState) {
        case WEAPON_FIRE:
            if (currentTime - *lastFireTime >= fireCooldown) {
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
