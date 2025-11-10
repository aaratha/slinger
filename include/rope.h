#pragma once

#include <SDL3/SDL.h>

#include "utils.h"
#include "camera.h"

class Rope {
  SDL_FPoint points[NUM_POINTS];
  SDL_FPoint prevPoints[NUM_POINTS];
  SDL_FPoint screenPoints[NUM_POINTS];
  float masses[NUM_POINTS];
  bool anchored = false;

public:
  Rope();
  ~Rope();
  SDL_FPoint get_end();
  SDL_FPoint get_anchor();
  void update(SDL_FPoint mousePos, bool isDragging);
  void draw(SDL_Renderer *renderer, Camera *camera);
};
