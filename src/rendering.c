#include "../header/rendering.h"
#include "../header/raindrops.h"

/**
 * drawScene - Function draws the scene to the screen
 * @renderer: SDL renderer
 * @texture: texture for the player
 * @posX: player position on the x axis
 * @posY: player position on the y axis
 * @dirX: player direction on the x axis
 * 
 * Return: void
 */
Raindrop raindrops[MAX_RAINDROPS];
void drawMiniMap(SDL_Renderer *renderer, double posX, double posY, double dirX, double dirY)
{
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			SDL_SetRenderDrawColor(renderer, worldMap[x][y] > 0 ? 255 : 0, 0, 0, 255);
			SDL_Rect rect = {x * MINIMAP_SCALE, y * MINIMAP_SCALE, MINIMAP_SCALE, MINIMAP_SCALE};
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	/*Draw player position*/
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect playerRect = {(int)(posX * MINIMAP_SCALE) - 2,
		(int)(posY * MINIMAP_SCALE) - 2, 4, 4};

	SDL_RenderFillRect(renderer, &playerRect);
	/*Draw direction line*/
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderDrawLine(renderer, (int)(posX * MINIMAP_SCALE), (int)(posY * MINIMAP_SCALE),
			(int)(posX * MINIMAP_SCALE + dirX * 5 * MINIMAP_SCALE),
			(int)(posY * MINIMAP_SCALE + dirY * 5 * MINIMAP_SCALE));
}

/**
 * drawRain - function to draw rain 
 * @renderer: SDL pointer to render scene
 * 
 * Return: void
 */
void drawRain(SDL_Renderer *renderer)
{
	/*Light blue color for raindrops*/
	SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255);

	for (int i = 0; i < MAX_RAINDROPS; i++)
	{
		/*Draw each raindrop as a small line*/
		SDL_RenderDrawLine(renderer, raindrops[i].x, raindrops[i].y,
				raindrops[i].x, raindrops[i].y + 5);
		/*Move rain drop down*/
		raindrops[i].y += raindrops[i].speed;

		/*If raindrop goes off the screen, reset its position to the top*/
		if (raindrops[i].y > SCREEN_HEIGHT)
		{
			raindrops[i].x = rand() % SCREEN_WIDTH;
			raindrops[i].y = -5;
		}
	}
}


/**
 * performRaycasting - Function performs raycasting and draws the scene
 * @renderer: SDL renderer
 * @wallTextures: array of textures for the walls
 * @skyTexture: pointer to sky texture 
 * @posX: player position on the x axis
 * @posY: player position on the y axis
 * @dirX: player direction on the x axis
 * @dirY: player direction on the y axis
 * @planeX: camera plane on the x axis
 * @planeY: camera plane on the y axis
 * 
 * Return: void
 */
void performRaycasting(SDL_Renderer *renderer, TextureData wallTextures[],
		TextureData *skyTexture, double posX, double posY, double dirX,
		double dirY, double planeX, double planeY)
{
	int lineHeight, drawStart, drawEnd, texID, texWidth, texHeight, texX;

	/* Draw the sky (top half of the screen)*/
	SDL_Rect skyRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};

	SDL_RenderCopy(renderer, skyTexture->texture, NULL, &skyRect);

	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		/*X-coordinate in camera space*/
		double cameraX = 2 * x / (double)SCREEN_WIDTH - 1;
		double rayDirX = dirX + planeX * cameraX;
		double rayDirY = dirY + planeY * cameraX;
		int mapX = (int)posX;
		int mapY = (int)posY;
		/*Length of ray from current position to next x or y-side*/
		double sideDistX, sideDistY;
		/*Length of ray from one x or y-side to the next x or y-side*/
		double deltaDistX = fabs(1 / rayDirX);
		double deltaDistY = fabs(1 / rayDirY);
		double perpWallDist;
		/*Direction to go in x and y (+1 or -1)*/
		int stepX, stepY;
		/*Was there a wall hit*/
		int hit = 0;
		/*Was a NS or a EW wall hit*/
		int side;

		/* Calculate step and initial sideDist*/
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (posX - mapX) * deltaDistX;
		} else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - posX) * deltaDistX;
		}

		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (posY - mapY) * deltaDistY;
		} else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - posY) * deltaDistY;
		}

		/*Perform DDA*/
		while (hit == 0)
		{
			/*Jump to next map square, OR in x-direction, OR in y-direction*/
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			} else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}

			/* Check if ray has hit a wall*/
			if (worldMap[mapX][mapY] > 0)
				hit = 1;
		}

		/*Calculate distance projected on camera direction
		 * (Euclidean distance will give fisheye effect!)*/
		if (side == 0)
			perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
		else
			perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;
		/* Calculate height of line to draw on screen*/
		lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
		/*Calculate lowest and highest pixel to fill in current stripe*/
		drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;

		if (drawStart < 0)
			drawStart = 0;
		drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;

		if (drawEnd >= SCREEN_HEIGHT)
			drawEnd = SCREEN_HEIGHT - 1;
		/*Texture coordinates*/
		/*Adjust for 0-based index*/
		texID = worldMap[mapX][mapY] - 1;
		/*Use dynamic width*/
		texWidth = wallTextures[texID].width;
		/*Use dynamic height*/
		texHeight = wallTextures[texID].height;
		SDL_Rect srcRect = {0, 0, texWidth, texHeight};
		SDL_Rect destRect = {x, drawStart, 1, drawEnd - drawStart};

		/* X-coordinate on the texture*/
		/*Where exactly the wall was hit*/
		double wallX;

		if (side == 0)
			wallX = posY + perpWallDist * rayDirY;
		else
			wallX = posX + perpWallDist * rayDirX;
		wallX -= floor((wallX));

		texX = (int)(wallX * (double)texWidth);

		if (side == 0 && rayDirX > 0)
			texX = texWidth - texX - 1;

		if (side == 1 && rayDirY < 0)
			texX = texWidth - texX - 1;

		srcRect.x = texX;

		/*Draw the wall slice*/

		SDL_RenderCopy(renderer, wallTextures[texID].texture,
				&srcRect, &destRect);
	}
}
/**
 * initializeRaindrops - function to make it rain
 * 
 * Return: void
 */

/* Initialize raindrops*/
void initializeRaindrops(void)
{
	int i;

	for (i = 0; i < MAX_RAINDROPS; i++)
	{
		raindrops[i].x = rand() % SCREEN_WIDTH;
		raindrops[i].y = rand() % SCREEN_HEIGHT;
		/*Rain speed between 0.5 and 1.0*/
		raindrops[i].speed = (rand() % 5 + 5) / 10.0f;
	}
}
