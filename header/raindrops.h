#ifndef RAINDROPS_H
#define RAINDROPS_H

typedef struct {
    float x, y, speed;
} Raindrop;

// Maximum number of raindrops
#define MAX_RAINDROPS 100
extern Raindrop raindrops[MAX_RAINDROPS];
#endif // RAINDROPS_H