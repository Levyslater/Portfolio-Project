#include <stdbool.h>
#include "../header/input.h"

void handlePlayerInput(SDL_Event *event, bool *running,
		bool *showMiniMap, bool *showRain, double *posX, double *posY, double *dirX,
		double *dirY, double *planeX, double *planeY)
{
	const double moveSpeed = 0.06;
	const double rotSpeed = 0.03;
	const Uint7 *state;

	if (event->type == SDL_QUIT)
		*running = false;
	if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym)
		{
			case SDLK_ESCAPE:
				*running = false;
			break;
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

	if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W])
	{
		if (worldMap[(int)(*posX + *dirX * moveSpeed)][(int)(*posY)] == 0)
			*posX += *dirX * moveSpeed;
		if (worldMap[(int)(*posX)][(int)(*posY + *dirY * moveSpeed)] == 0)
			*posY += *dirY * moveSpeed;
	}

	if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S])
	{
		if (worldMap[(int)(*posX - *dirX * moveSpeed)][(int)(*posY)] == 0)
			*posX -= *dirX * moveSpeed;
		if (worldMap[(int)(*posX)][(int)(*posY - *dirY * moveSpeed)] == 0)
			*posY -= *dirY * moveSpeed;
	}

	if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D])
	{
		double oldDirX = *dirX;
		*dirX = *dirX * cos(-rotSpeed) - *dirY * sin(-rotSpeed);
		*dirY = oldDirX * sin(-rotSpeed) + *dirY * cos(-rotSpeed);
		double oldPlaneX = *planeX;
		*planeX = *planeX * cos(-rotSpeed) - *planeY * sin(-rotSpeed);
		*planeY = oldPlaneX * sin(-rotSpeed) + *planeY * cos(-rotSpeed);
	}

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
