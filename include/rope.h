#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "camera.h"
#include "utils.h"

using namespace std;

class Rope {
  vector<float> x_curr;
  vector<float> y_curr;
  vector<float> x_prev;
  vector<float> y_prev;
  vector<SDL_FPoint> screen_points;
  vector<float> masses;
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
  vector<float> &get_x();
  vector<float> &get_y();
  void solve_collisions(float &y);
  void solve_physics();
  void forward_constraints();
  void backward_constraints();
  void solve_constraints();
  void update(SDL_FPoint mousePos);
  void draw(SDL_Renderer *renderer, Camera &camera);
};
