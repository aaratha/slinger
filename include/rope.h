#pragma once

#include <SDL3/SDL.h>

#include "camera.h"
#include "utils.h"

class Rope {
  float x_curr[NUM_POINTS];
  float y_curr[NUM_POINTS];
  float x_prev[NUM_POINTS];
  float y_prev[NUM_POINTS];
  SDL_FPoint screen_points[NUM_POINTS];
  float masses[NUM_POINTS];
  bool anchored = false;
  int brightness = 0;
  float end_speed;

public:
  Rope();
  ~Rope();
  SDL_FPoint get_end();
  SDL_FPoint get_anchor();
  float get_altitude();
  float get_speed();
  void solve_collisions(float &y);
  void solve_physics();
  void forward_constraints();
  void backward_constraints();
  void solve_constraints();
  void update(SDL_FPoint mousePos);
  void draw(SDL_Renderer *renderer, Camera *camera);
};
