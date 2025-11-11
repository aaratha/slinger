#pragma once

#include <SDL3/SDL.h>
#include <cmath>

#define NUM_POINTS 20
#define BALL_RADIUS 10.0f
#define POINT_SPACING 10.0f
#define GRAVITY 1000.0f
#define DT 0.016f
#define CONSTRAINT_ITERATIONS 80
#define DAMPING 1.0f
#define AIR_RESISTANCE 0.02f
#define CAMERA_LERP 0.2f
#define FLOOR_FRICTION 2000.0f;
#define FLOOR_HEIGHT 30

SDL_FPoint operator*(float scalar, const SDL_FPoint &point);

SDL_FPoint operator/(const SDL_FPoint &point, float scalar);

SDL_FPoint operator+(const SDL_FPoint &a, const SDL_FPoint &b);

SDL_FPoint operator-(const SDL_FPoint &a, const SDL_FPoint &b);

SDL_FPoint &operator+=(SDL_FPoint &a, const SDL_FPoint &b);

float lerp1D(float a, float b, float t);
SDL_FPoint lerp2D(SDL_FPoint a, SDL_FPoint b, float t);

float point_distance(const SDL_FPoint &a, const SDL_FPoint &b);

void draw_circle(SDL_Renderer *renderer, int32_t centerX, int32_t centerY,
                 int32_t radius);
