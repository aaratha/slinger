#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "camera.h"

using namespace std;

enum class EnemyType { Base };

class EnemySystem {
  int count;
  float timer;
  float spawn_time;

  vector<EnemyType> enemy_type;
  vector<float> x_curr;
  vector<float> y_curr;
  vector<float> x_prev;
  vector<float> y_prev;
  vector<float> attraction;
  vector<float> mass;
  vector<float> radius;
  vector<float> max_vel;

public:
  EnemySystem();
  ~EnemySystem();

  void add(EnemyType type, float x, float y);
  void update(Camera &camera, vector<float> &x_rope, vector<float> &y_rope);
  void draw(SDL_Renderer *renderer, Camera &camera);
};
