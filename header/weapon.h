#ifndef WEAPON_H
#define WEAPON_H

#include "main.h"

WeaponState handleWeaponInput(bool* firing, WeaponState currentWeaponState);
void renderWeapon(SDL_Renderer* renderer, TextureData* weaponTextures, WeaponState weaponState);
void updateWeaponState(WeaponState* weaponState, Uint32* lastFireTime, Uint32 fireCooldown);

#endif
