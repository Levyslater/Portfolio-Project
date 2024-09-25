#include "../header/main.h"
#include "../header/textures.h"
#include "../header/input.h"
#include "../header/rendering.h"
#include "../header/weapon.h"

/**
 * main - main function in the code
 * @argc: arguments counter
 * @argv: pointer to argument array
 * 
 * Return: null
 */

Mix_Chunk *shotgunSound = NULL;
int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	bool showRain;
	bool running;
	bool showMiniMap;

	/* Initialize SDL and create a window */
	if (SDL_Init(SDL_INIT_VIDEO) < 0 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		fprintf(stderr, "SDL_Init or IMG_Init failed: %s\n", SDL_GetError());
		return (1);
	}
	/*Initialize SDL_mixer*/

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return (EXIT_FAILURE);
	}

    /* Initialize the SDL window*/
	SDL_Window * window = SDL_CreateWindow("SDL Raycasting", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
			SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!window)
	/*Raise error if window creation was unsuccesful*/
	{
		fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
		return (1);
	}

    /* Create a renderer for the window */
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	/*Raise error if renderer creation was unsuccessful*/
	{
		fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
		return (1);
	}

	/*Load map and initialize player position and direction*/
	loadMap("./resources/map.txt");
	/* Initial player position*/
	double posX = 22, posY = 12;
	/* Initial player direction */
	double dirX = -1, dirY = 0;
	/* 2D raycaster version of camera plane */
	double planeX = 0, planeY = 0.66;

	/*Load textures for walls, sky, and weapon */
	TextureData wallTextures[5];
	wallTextures[0] = loadTextureWithDimensions("./resources/textures/1.png", renderer);
	wallTextures[1] = loadTextureWithDimensions("./resources/textures/2.png", renderer);
	wallTextures[2] = loadTextureWithDimensions("./resources/textures/3.png", renderer);
	wallTextures[3] = loadTextureWithDimensions("./resources/textures/4.png", renderer);
	wallTextures[4] = loadTextureWithDimensions("./resources/textures/5.png", renderer);
	TextureData skyTexture = loadTextureWithDimensions("./resources/textures/sky.png", renderer);
	TextureData weaponTextures[4];
	weaponTextures[0] = loadTextureWithDimensions("./resources/sprites/weapon/shotgun/0.png", renderer);
	weaponTextures[1] = loadTextureWithDimensions("./resources/sprites/weapon/shotgun/1.png", renderer);
	weaponTextures[2] = loadTextureWithDimensions("./resources/sprites/weapon/shotgun/4.png", renderer);
	weaponTextures[3] = loadTextureWithDimensions("./resources/sprites/weapon/shotgun/5.png", renderer);

	/* Initialize raindrops */
	initializeRaindrops();

	/* Load audio files */
	Mix_Music *themeMusic = Mix_LoadMUS("./resources/sound/theme.mp3");

	shotgunSound = Mix_LoadWAV("./resources/sound/shotgun.wav");

	if (!themeMusic)
	/*raise an error if music was not loaded*/
	{
		fprintf(stderr, "Failed to load theme music! SDL_mixer Error: %s\n", Mix_GetError());
		return (EXIT_FAILURE);
	}

	if (!shotgunSound)
	/*raise an error if sound was not loaded*/
	{
		fprintf(stderr, "Failed to load gunshot sound! SDL_mixer Error: %s\n", Mix_GetError());
		return (EXIT_FAILURE);
	}
	/*Play theme music in a loop*/
	Mix_PlayMusic(themeMusic, -1);
	/*Toggle rain effect*/
	showRain = false;
	running = true;
	showMiniMap = true;
	/*Weapon default state*/
	WeaponState weaponState = WEAPON_AIM;
	/*Store the time when the weapon was last fired*/
	Uint32 lastFireTime = 0;
	/*Time in (ms) between firing and recoil*/
	Uint32 fireCooldown = 300;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			handlePlayerInput(&event, &running, &showMiniMap, &showRain,
					 &posX, &posY, &dirX, &dirY, &planeX, &planeY);
			/*Handle weapon firing */
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
			{
				if (weaponState == WEAPON_AIM)
				{
					weaponState = WEAPON_FIRE;
					/*Play gunshot sound*/
					Mix_PlayChannel(-1, shotgunSound, 0);
					/*Update last fired time to current time*/
					lastFireTime = SDL_GetTicks();
				}
			}
		}

		/*Update weapon state*/
		updateWeaponState(&weaponState, &lastFireTime, fireCooldown);

		/*Clear screen and render scene*/
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		performRaycasting(renderer, wallTextures, &skyTexture, posX, posY, dirX,
				dirY, planeX, planeY);

		/*Render mini-map if enabled*/
		if (showMiniMap)
			drawMiniMap(renderer, posX, posY, dirX, dirY);

		/*Render weapon*/
		renderWeapon(renderer, weaponTextures, weaponState);

		/*Update weapon state*/
		if (weaponState == WEAPON_FIRE)
		{
			/*Delay time to manage weapon cooldown time and recoil*/
			SDL_Delay(100);
			/*Return weapon to default aim state after firing*/
			weaponState = WEAPON_AIM;
		}
		/* Draw rain if enabled*/
		if (showRain)
			drawRain(renderer);
		/*Present everything on the screen*/
		SDL_RenderPresent(renderer);
		/*Manage frame rates(for 60fps)*/
		SDL_Delay(16);
	}

	/* Clean up resources by freeing the heap */
	Mix_FreeMusic(themeMusic);
	Mix_FreeChunk(shotgunSound);

	/*free wall textures recursively*/
	for (int i = 0; i < 5; i++)
		SDL_DestroyTexture(wallTextures[i].texture);

	/*free the weapon textures recursively*/
	for (int i = 0; i < 4; i++)
	{
		SDL_DestroyTexture(weaponTextures[i].texture);
	}
	SDL_DestroyTexture(skyTexture.texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return (0);
}
