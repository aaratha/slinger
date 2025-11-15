#include "utils.h"
#include <cmath>

SDL_FPoint operator*(float scalar, const SDL_FPoint &point) {
  return SDL_FPoint{scalar * point.x, scalar * point.y};
}

SDL_FPoint operator/(const SDL_FPoint &point, float scalar) {
  return SDL_FPoint{point.x / scalar, point.y / scalar};
}

SDL_FPoint operator+(const SDL_FPoint &a, const SDL_FPoint &b) {
  return SDL_FPoint{a.x + b.x, a.y + b.y};
}

SDL_FPoint operator-(const SDL_FPoint &a, const SDL_FPoint &b) {
  return SDL_FPoint{a.x - b.x, a.y - b.y};
}

SDL_FPoint operator+=(SDL_FPoint &a, const SDL_FPoint &b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

SDL_FPoint operator-=(SDL_FPoint &a, const SDL_FPoint &b) {
  a.x -= b.x;
  a.y -= b.y;
  return a;
}

SDL_FPoint operator*=(SDL_FPoint &a, float scalar) {
  a.x *= scalar;
  a.y *= scalar;
  return a;
}

float lerp1D(float a, float b, float t) { return a + t * (b - a); }

SDL_FPoint lerp2D(SDL_FPoint a, SDL_FPoint b, float t) {
  return a + t * (b - a);
}

float point_distance(const SDL_FPoint &a, const SDL_FPoint &b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return sqrtf(dx * dx + dy * dy);
}

float magnitude(SDL_FPoint &a) { return sqrtf(a.x * a.x + a.y * a.y); }

void draw_circle(SDL_Renderer *renderer, int32_t centerX, int32_t centerY,
                 int32_t radius) {
  for (int w = 0; w < radius * 2; w++) {
    for (int h = 0; h < radius * 2; h++) {
      int dx = radius - w; // horizontal offset
      int dy = radius - h; // vertical offset
      if ((dx * dx + dy * dy) <= (radius * radius)) {
        SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
      }
    }
  }
}
