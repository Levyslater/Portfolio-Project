#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>  // Include SDL_mixer for audio

// Screen dimensions and map size
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define MINIMAP_SCALE 5 // Scale factor for mini-map size

// Weapon state definitions
typedef enum {
    WEAPON_AIM,
    WEAPON_FIRE,
    WEAPON_RECOIL,
    WEAPON_RELOAD
} WeaponState;

// Sound effect variables
Mix_Chunk* shotgunSound = NULL;  // Use Mix_Chunk for short sound effects

// Structure to store texture and its dimensions
typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} TextureData;

int worldMap[MAP_WIDTH][MAP_HEIGHT];

// Raindrop structure
typedef struct {
    float x, y, speed;
} Raindrop;

// Maximum number of raindrops
#define MAX_RAINDROPS 100
Raindrop raindrops[MAX_RAINDROPS];

// Load texture from file
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

// Function to load a texture and get its dimensions
TextureData loadTextureWithDimensions(const char* file, SDL_Renderer* renderer) {
    TextureData texData;
    texData.texture = loadTexture(file, renderer); // Load texture
    SDL_QueryTexture(texData.texture, NULL, NULL, &texData.width, &texData.height); // Get dimensions
    return texData;
}

// Load map from file
void loadMap(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open map file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (fscanf(file, "%d", &worldMap[x][y]) != 1) {
                fprintf(stderr, "Error reading map data at %d, %d\n", x, y);
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file);
}

// Initialize raindrops
void initializeRaindrops() {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].x = rand() % SCREEN_WIDTH;
        raindrops[i].y = rand() % SCREEN_HEIGHT;
        raindrops[i].speed = (rand() % 5 + 5) / 10.0f; // Speed between 0.5 and 1.0
    }
}

// Draw the mini-map
void drawMiniMap(SDL_Renderer* renderer, double posX, double posY, double dirX, double dirY) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_SetRenderDrawColor(renderer, worldMap[x][y] > 0 ? 255 : 0, 0, 0, 255);
            SDL_Rect rect = {x * MINIMAP_SCALE, y * MINIMAP_SCALE, MINIMAP_SCALE, MINIMAP_SCALE};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Draw player position
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect playerRect = {(int)(posX * MINIMAP_SCALE) - 2, (int)(posY * MINIMAP_SCALE) - 2, 4, 4};
    SDL_RenderFillRect(renderer, &playerRect);

    // Draw direction line
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawLine(renderer, (int)(posX * MINIMAP_SCALE), (int)(posY * MINIMAP_SCALE),
                       (int)(posX * MINIMAP_SCALE + dirX * 5 * MINIMAP_SCALE),
                       (int)(posY * MINIMAP_SCALE + dirY * 5 * MINIMAP_SCALE));
}

// Handle weapon state and render the weapon on the screen
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

// Draw rain particles
void drawRain(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255); // Light blue color for raindrops
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        // Draw each raindrop as a small line
        SDL_RenderDrawLine(renderer, raindrops[i].x, raindrops[i].y, raindrops[i].x, raindrops[i].y + 5);
        raindrops[i].y += raindrops[i].speed; // Move raindrop down

        // If raindrop goes off the screen, reset its position to the top
        if (raindrops[i].y > SCREEN_HEIGHT) {
            raindrops[i].x = rand() % SCREEN_WIDTH;
            raindrops[i].y = -5;
        }
    }
}

void handlePlayerInput(SDL_Event* event, bool* running, bool* showMiniMap, bool* showRain, double* posX, double* posY, double* dirX, double* dirY, double* planeX, double* planeY) {
    const double moveSpeed = 0.06;
    const double rotSpeed = 0.03;

    if (event->type == SDL_QUIT) *running = false;
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE: *running = false; break;
            case SDLK_m: *showMiniMap = !*showMiniMap; break;
            case SDLK_r: *showRain = !*showRain; break; // Toggle rain
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {
        if (worldMap[(int)(*posX + *dirX * moveSpeed)][(int)(*posY)] == 0) *posX += *dirX * moveSpeed;
        if (worldMap[(int)(*posX)][(int)(*posY + *dirY * moveSpeed)] == 0) *posY += *dirY * moveSpeed;
    }

    if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {
        if (worldMap[(int)(*posX - *dirX * moveSpeed)][(int)(*posY)] == 0) *posX -= *dirX * moveSpeed;
        if (worldMap[(int)(*posX)][(int)(*posY - *dirY * moveSpeed)] == 0) *posY -= *dirY * moveSpeed;
    }

    if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
        double oldDirX = *dirX;
        *dirX = *dirX * cos(-rotSpeed) - *dirY * sin(-rotSpeed);
        *dirY = oldDirX * sin(-rotSpeed) + *dirY * cos(-rotSpeed);
        double oldPlaneX = *planeX;
        *planeX = *planeX * cos(-rotSpeed) - *planeY * sin(-rotSpeed);
        *planeY = oldPlaneX * sin(-rotSpeed) + *planeY * cos(-rotSpeed);
    }

    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
        double oldDirX = *dirX;
        *dirX = *dirX * cos(rotSpeed) - *dirY * sin(rotSpeed);
        *dirY = oldDirX * sin(rotSpeed) + *dirY * cos(rotSpeed);
        double oldPlaneX = *planeX;
        *planeX = *planeX * cos(rotSpeed) - *planeY * sin(rotSpeed);
        *planeY = oldPlaneX * sin(rotSpeed) + *planeY * cos(rotSpeed);
    }
}

// Handle weapon input and state transition
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

// Perform raycasting and render the scene
// Perform raycasting and render the scene
void performRaycasting(SDL_Renderer* renderer, TextureData wallTextures[], TextureData* skyTexture, double posX, double posY, double dirX, double dirY, double planeX, double planeY) {
    // Draw the sky (top half of the screen)
    SDL_Rect skyRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    SDL_RenderCopy(renderer, skyTexture->texture, NULL, &skyRect);

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        double cameraX = 2 * x / (double)SCREEN_WIDTH - 1; // X-coordinate in camera space
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        int mapX = (int)posX;
        int mapY = (int)posY;

        // Length of ray from current position to next x or y-side
        double sideDistX, sideDistY;

        // Length of ray from one x or y-side to the next x or y-side
        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);
        double perpWallDist;

        // Direction to go in x and y (+1 or -1)
        int stepX, stepY;

        int hit = 0; // Was there a wall hit?
        int side; // Was a NS or a EW wall hit?

        // Calculate step and initial sideDist
        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        // Perform DDA
        while (hit == 0) {
            // Jump to next map square, OR in x-direction, OR in y-direction
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            // Check if ray has hit a wall
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        // Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
        if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
        else          perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

        // Calculate height of line to draw on screen
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        // Texture coordinates
        int texID = worldMap[mapX][mapY] - 1; // Adjusting for 0-based index
        int texWidth = wallTextures[texID].width;  // Use dynamic width
        int texHeight = wallTextures[texID].height; // Use dynamic height

        SDL_Rect srcRect = {0, 0, texWidth, texHeight};
        SDL_Rect destRect = {x, drawStart, 1, drawEnd - drawStart};

        // X-coordinate on the texture
        double wallX; // Where exactly the wall was hit
        if (side == 0) wallX = posY + perpWallDist * rayDirY;
        else           wallX = posX + perpWallDist * rayDirX;
        wallX -= floor((wallX));

        int texX = (int)(wallX * (double)texWidth);
        if (side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
        if (side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

        srcRect.x = texX;

        // Draw the wall slice
        SDL_RenderCopy(renderer, wallTextures[texID].texture, &srcRect, &destRect);
    }
}
// Function to update weapon state
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


int main(int argc, char* argv[]) {
    // Initialize SDL and create a window
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_Init or IMG_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("SDL Raycasting", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }

    // Load map and initialize player position and direction
    loadMap("map.txt");
    double posX = 22, posY = 12; // Initial player position
    double dirX = -1, dirY = 0; // Initial player direction vector
    double planeX = 0, planeY = 0.66; // 2D raycaster version of camera plane

    // Load textures for walls, sky, and weapon
    TextureData wallTextures[5];
    wallTextures[0] = loadTextureWithDimensions("../resources/textures/1.png", renderer);
    wallTextures[1] = loadTextureWithDimensions("../resources/textures/2.png", renderer);
    wallTextures[2] = loadTextureWithDimensions("../resources/textures/3.png", renderer);
    wallTextures[3] = loadTextureWithDimensions("../resources/textures/4.png", renderer);
    wallTextures[4] = loadTextureWithDimensions("../resources/textures/5.png", renderer);
    TextureData skyTexture = loadTextureWithDimensions("../resources/textures/sky.png", renderer);
    TextureData weaponTextures[4];
    weaponTextures[0] = loadTextureWithDimensions("../resources/sprites/weapon/shotgun/0.png", renderer);
    weaponTextures[1] = loadTextureWithDimensions("../resources/sprites/weapon/shotgun/1.png", renderer);
    weaponTextures[2] = loadTextureWithDimensions("../resources/sprites/weapon/shotgun/4.png", renderer);
    weaponTextures[3] = loadTextureWithDimensions("../resources/sprites/weapon/shotgun/5.png", renderer);

    // Initialize raindrops
    initializeRaindrops();

    // Load audio files
    Mix_Music* themeMusic = Mix_LoadMUS("../resources/sound/theme.mp3");
    shotgunSound = Mix_LoadWAV("../resources/sound/shotgun.wav");

    if (!themeMusic) {
        fprintf(stderr, "Failed to load theme music! SDL_mixer Error: %s\n", Mix_GetError());
        return EXIT_FAILURE;
    }
    if (!shotgunSound) {
        fprintf(stderr, "Failed to load gunshot sound! SDL_mixer Error: %s\n", Mix_GetError());
        return EXIT_FAILURE;
    }

    Mix_PlayMusic(themeMusic, -1);  // Play theme music in a loop

    bool running = true;
    bool showMiniMap = true;
    bool showRain = false; // Toggle rain effect
    bool firing = false;
    WeaponState weaponState = WEAPON_AIM;
    Uint32 lastFireTime = 0;  // Store the time when the weapon was last fired
    Uint32 fireCooldown = 300; // Time (ms) between firing and recoil

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handlePlayerInput(&event, &running, &showMiniMap, &showRain, &posX, &posY, &dirX, &dirY, &planeX, &planeY);

            // Handle weapon firing
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                if (weaponState == WEAPON_AIM) {
                    weaponState = WEAPON_FIRE;
                    Mix_PlayChannel(-1, shotgunSound, 0);  // Play gunshot sound
                    lastFireTime = SDL_GetTicks(); // Set last fire time to current time
                }
            }
        }
    

        // Handle weapon input and state transition
        //weaponState = handleWeaponInput(&firing, weaponState);

        // Update weapon state using the new function
        updateWeaponState(&weaponState, &lastFireTime, fireCooldown);

        // Clear screen and render scene
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        performRaycasting(renderer, wallTextures, &skyTexture, posX, posY, dirX, dirY, planeX, planeY);

        // Render mini-map if enabled
        if (showMiniMap) drawMiniMap(renderer, posX, posY, dirX, dirY);

        // Render weapon
        renderWeapon(renderer, weaponTextures, weaponState);

        // Update weapon state
        if (weaponState == WEAPON_FIRE) {
            SDL_Delay(100); // Adjust the delay value if needed
            weaponState = WEAPON_RECOIL; // Return to aim state after firing
            SDL_Delay(100);
            weaponState = WEAPON_RELOAD; // Return to aim state after firing
            SDL_Delay(100);
            weaponState = WEAPON_AIM; // Return to aim state after firing
        }
        // Draw rain if enabled
        if (showRain) drawRain(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Control frame rate
    }

    // Clean up resources
    Mix_FreeMusic(themeMusic);
    Mix_FreeChunk(shotgunSound);
    for (int i = 0; i < 5; i++) {
        SDL_DestroyTexture(wallTextures[i].texture);
    }
    for (int i = 0; i < 4; i++) {
        SDL_DestroyTexture(weaponTextures[i].texture);
    }
    SDL_DestroyTexture(skyTexture.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}