#include <stdio.h>
#include <stdlib.h>
#include "../header/map.h"
#include "../header/constants.h"

int worldMap[MAP_WIDTH][MAP_HEIGHT];

void loadMap(const char *filename)
{
	FILE *file = fopen(filename, "r");

	if (!file)
	{
		fprintf(stderr, "Failed to open map file: %s\n", filename);
		exit(EXIT_FAILURE);
	}
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			if (fscanf(file, "%d", &worldMap[x][y]) != 1)
			{
				fprintf(stderr, "Error reading map data at %d, %d\n", x, y);
				fclose(file);
				exit(EXIT_FAILURE);
			}
		}
	}
	fclose(file);
}
