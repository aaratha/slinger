#pragma once

#include <SDL3/SDL.h>

class Camera {
  SDL_FPoint pos;
  float speed;

public:
  Camera();
  ~Camera();

  SDL_FPoint worldToScreen(const SDL_FPoint &world);

  SDL_FPoint screenToWorld(const SDL_FPoint &screen) const;

  SDL_FPoint get_pos();

  void update(SDL_FPoint anchor, SDL_FPoint end);
};
