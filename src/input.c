#include <stdbool.h>
#include "../header/input.h"

/**
 * handlePLayerInput - Function handles player inputs like movement
 * @event: SDL event handler
 * @running: game loop control variable
 * @showMiniMap: variable to help toggle minimap display
 * @showRain: variable to help toggle rain
 * @posX: player position on the x axis
 * @posY: player position on the y axis
 * @dirX: player direction on the x axis
 * @dirY: player direction on the y axis
 * @planeX: camera plane on the x axis
 * @planeY: camera plane on the y axis
 * 
 * Return: has no return value to the caller function
 */
void handlePlayerInput(SDL_Event *event, bool *running,
		bool *showMiniMap, bool *showRain, double *posX, double *posY, double *dirX,
		double *dirY, double *planeX, double *planeY)
{
	/*Declare player movement speed*/
	const double moveSpeed = 0.06;
	const double rotSpeed = 0.03;
	const Uint8 *state;

	/*end game loop when player quits the game*/
	if (event->type == SDL_QUIT)
		*running = false;

    /*Toggle minimap and rain display*/
	if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym)
		{
			case SDLK_ESCAPE:
				*running = false;
			break;
			/* Toggle minimap */
			case SDLK_m:
					  *showMiniMap = !*showMiniMap;
			break;
			/* Toggle rain */
			case SDLK_r:
				     *showRain = !*showRain;
			break;
		}
	}

	state = SDL_GetKeyboardState(NULL);

	/*Move the player up when the W or arrow up key is pressed*/
	if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W])
	{
		if (worldMap[(int)(*posX + *dirX * moveSpeed)][(int)(*posY)] == 0)
			*posX += *dirX * moveSpeed;
		if (worldMap[(int)(*posX)][(int)(*posY + *dirY * moveSpeed)] == 0)
			*posY += *dirY * moveSpeed;
	}

    /*Move the player down when the S or arrow down key is pressed*/
	if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S])
	{
		if (worldMap[(int)(*posX - *dirX * moveSpeed)][(int)(*posY)] == 0)
			*posX -= *dirX * moveSpeed;
		if (worldMap[(int)(*posX)][(int)(*posY - *dirY * moveSpeed)] == 0)
			*posY -= *dirY * moveSpeed;
	}

    /*Rotate the player left when the A or arrow left key is pressed*/
	if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D])
	{
		double oldDirX = *dirX;
		*dirX = *dirX * cos(-rotSpeed) - *dirY * sin(-rotSpeed);
		*dirY = oldDirX * sin(-rotSpeed) + *dirY * cos(-rotSpeed);
		double oldPlaneX = *planeX;
		*planeX = *planeX * cos(-rotSpeed) - *planeY * sin(-rotSpeed);
		*planeY = oldPlaneX * sin(-rotSpeed) + *planeY * cos(-rotSpeed);
	}

    /*Rotate the player right when the D or arrow right key is pressed*/
	if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A])
	{
		double oldDirX = *dirX;
		*dirX = *dirX * cos(rotSpeed) - *dirY * sin(rotSpeed);
		*dirY = oldDirX * sin(rotSpeed) + *dirY * cos(rotSpeed);
		double oldPlaneX = *planeX;
		*planeX = *planeX * cos(rotSpeed) - *planeY * sin(rotSpeed);
		*planeY = oldPlaneX * sin(rotSpeed) + *planeY * cos(rotSpeed);
	}
}
