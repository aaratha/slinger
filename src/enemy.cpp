#include "enemy.h"

#include "globals.h"
#include "utils.h"
#include <cstdint>
#include <sys/types.h>

EnemyGrid::EnemyGrid(float enemy_radius) { cell_size = enemy_radius * 4.0f; }

EnemyGrid::~EnemyGrid() { clear(); }

void EnemyGrid::clear() { grid.clear(); }

void EnemyGrid::add(float x, float y, int index) {
  int cell_x = floorf(x / cell_size);
  int cell_y = floorf(y / cell_size);
  uint64_t cell_key = (uint64_t)(uint32_t)cell_x << 32 | (uint32_t)cell_y;

  grid[cell_key].push_back(index);
}

unordered_map<uint64_t, vector<int>> &EnemyGrid::get_grid() { return grid; }

float EnemyGrid::get_cell_size() { return cell_size; }

EnemySystem::EnemySystem() : enemy_grid(gGS.enemy_radius) {
  count = 0;
  timer = 0.0f;
  spawn_time = 2.0f;
}

EnemySystem::~EnemySystem() {}

void EnemySystem::add(EnemyType type, float x, float y) {
  if (type == EnemyType::Base) {
    enemy_type.push_back(EnemyType::Base);

    x_curr.push_back(x);
    y_curr.push_back(y);
    x_prev.push_back(x);
    y_prev.push_back(y);

    attraction.push_back(1000.0f);
    radius.push_back(gGS.enemy_radius);
    mass.push_back(1.0f);
    max_vel.push_back(10.0f);

    count += 1;
  }
}

void EnemySystem::update(Camera &camera, vector<float> &x_rope,
                         vector<float> &y_rope) {
  // spawn process
  timer += DT;
  if (timer >= spawn_time) {
    timer = 0.0f;

    SDL_FPoint p = camera.rand_point_in_view();
    add(EnemyType::Base, p.x, p.y);
  }

  // physics process
  for (int i = 0; i < count; i++) {

    // main physics
    SDL_FPoint attraction_vec = {x_rope[0] - x_curr[i], y_rope[0] - y_curr[i]};
    float attraction_vec_mag = magnitude(attraction_vec);
    SDL_FPoint attraction_dir = attraction_vec / attraction_vec_mag;

    SDL_FPoint f = attraction[i] * attraction_dir;

    SDL_FPoint vel = {x_curr[i] - x_prev[i], y_curr[i] - y_prev[i]};
    vel *= DAMPING;

    // clamp velocity to vel_mag
    float vel_mag = magnitude(vel);
    if (vel_mag > max_vel[i]) {
      float ratio = max_vel[i] / vel_mag;
      vel *= ratio;
    }

    SDL_FPoint f_drag = -AIR_RESISTANCE * vel;
    f += f_drag;

    SDL_FPoint accel = f / mass[i];

    float x_new = x_curr[i] + vel.x + DT * DT * accel.x;
    x_prev[i] = x_curr[i];
    x_curr[i] = x_new;

    float y_new = y_curr[i] + vel.y + DT * DT * accel.y;
    y_prev[i] = y_curr[i];
    y_curr[i] = y_new;

    // collisions with ground
    if (y_curr[i] >= gGS.winH - FLOOR_HEIGHT) {
      float diff = y_curr[i] - (gGS.winH - FLOOR_HEIGHT);
      y_curr[i] -= diff;
    }

    for (int iter = 0; iter < 8; iter++) {
      // collisions with rope
      for (int j = 0; j < NUM_POINTS - 1; j++) {
        // solve point inside circle
        float x_diff = x_rope[j] - x_curr[i];
        float y_diff = y_rope[j] - y_curr[i];
        if (x_diff * x_diff + y_diff * y_diff <= radius[i] * radius[i]) {
          SDL_FPoint n_vec = {x_diff, y_diff};
          float n_mag = magnitude(n_vec);
          SDL_FPoint normal = n_vec / n_mag;

          SDL_FPoint correction = (radius[i] - n_mag) / 2.0f * normal;
          x_rope[j] += correction.x;
          y_rope[j] += correction.y;
          x_curr[i] -= correction.x;
          y_curr[i] -= correction.y;
        }
      }
    }
  }

  // clear grid
  enemy_grid.clear();

  // add all enemies to grid
  for (int i = 0; i < count; i++) {
    enemy_grid.add(x_curr[i], y_curr[i], i);
  }

  // collisions with eachother
  unordered_map<uint64_t, vector<int>> &grid = enemy_grid.get_grid();
  for (int i = 0; i < count; ++i) {
    int cx = (int)SDL_floorf(x_curr[i] / enemy_grid.get_cell_size());
    int cy = (int)SDL_floorf(y_curr[i] / enemy_grid.get_cell_size());

    for (int dy = -1; dy <= 1; ++dy) {
      for (int dx = -1; dx <= 1; ++dx) {
        int nx = cx + dx, ny = cy + dy;
        uint64_t key = (uint64_t)(uint32_t)nx << 32 | (uint32_t)ny;
        auto it = grid.find(key);
        if (it == grid.end())
          continue;
        const vector<int> &bucket = it->second;
        for (int j : bucket) {
          if (j <= i)
            continue; // avoid double work
          //  check if points are touching
          SDL_FPoint vec = {x_curr[i] - x_curr[j], y_curr[i] - y_curr[j]};
          float dist = SDL_sqrt(vec.x * vec.x + vec.y * vec.y);
          SDL_FPoint dir = vec / dist;
          float sum = radius[i] + radius[j];
          if (dist <= sum) {
            float diff = sum - dist;
            SDL_FPoint correction = (diff * 0.5f) * dir;

            x_curr[i] += correction.x;
            y_curr[i] += correction.y;
            x_curr[j] -= correction.x;
            y_curr[j] -= correction.y;
          }
        }
      }
    }
  }
}

void EnemySystem::draw(SDL_Renderer *renderer, Camera &camera) {
  for (int i = 0; i < count; i++) {
    SDL_FPoint screen_pos = camera.worldToScreen({x_curr[i], y_curr[i]});
    draw_circle(renderer, screen_pos.x, screen_pos.y, radius[i]);
  }
}
