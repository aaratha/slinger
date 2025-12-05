#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>

#include "camera.h"

using namespace std;

enum class EnemyType { Base };

class EnemyGrid {
  float cell_size;
  unordered_map<uint64_t, vector<int>> grid;

public:
  EnemyGrid(float enemy_radius);
  ~EnemyGrid();

  void clear();
  void add(float x, float y, int index);
  float get_cell_size() const;

  unordered_map<uint64_t, vector<int>> &get_grid();
  float get_cell_size();
};

class EnemySystem {
  int count;
  float timer;
  float spawn_time;

  EnemyGrid enemy_grid;

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
