#pragma once

#include <SDL3/SDL.h>

#include "camera.h"
#include "utils.h"

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
  void solve_collisions(SDL_FPoint *point);
  void solve_physics();
  void forward_constraints();
  void backward_constraints();
  void solve_constraints();
  void update(SDL_FPoint mousePos);
  void draw(SDL_Renderer *renderer, Camera *camera);
};
