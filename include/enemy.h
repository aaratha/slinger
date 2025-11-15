#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "camera.h"

using namespace std;

class Enemy {
  SDL_FPoint pos;
  SDL_FPoint prev_pos;
  float attraction;
  float radius;
  float mass;

public:
  Enemy(SDL_FPoint p);
  ~Enemy();

  float get_attraction();
  void update(SDL_FPoint anchor);
  void draw(SDL_Renderer *renderer, Camera *camera);
};

class Enemies {
  float timer;
  float spawn_time;
  vector<unique_ptr<Enemy>> enemies;

public:
  Enemies();
  ~Enemies();

  void update(Camera *camera, SDL_FPoint anchor);
  void draw(SDL_Renderer *renderer, Camera *camera);
};
