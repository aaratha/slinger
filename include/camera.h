#pragma once

#include <SDL3/SDL.h>

class Camera {
  SDL_FPoint pos;

public:
  Camera();
  ~Camera();

  SDL_FPoint worldToScreen(const SDL_FPoint &world, int winW, int winH);

  SDL_FPoint screenToWorld(const SDL_FPoint &screen, int winW, int winH) const;

  SDL_FPoint get_pos();

  void update(SDL_FPoint anchor, SDL_FPoint end);
};
