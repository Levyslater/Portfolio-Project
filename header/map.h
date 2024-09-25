// map.h
#ifndef MAP_H
#define MAP_H

#include "constants.h" // Include the constants for map dimensions

extern int worldMap[MAP_WIDTH][MAP_HEIGHT]; // Map array declaration

// Function declarations
void loadMap(const char* filename);

#endif // MAP_H
