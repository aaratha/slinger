#include "enemy.h"

#include "globals.h"
#include "utils.h"

EnemySystem::EnemySystem() {
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
    radius.push_back(10.0f);
    mass.push_back(1.0f);
    max_vel.push_back(10.0f);
  }
}

void EnemySystem::update(Camera *camera, SDL_FPoint anchor) {
  // spawn process
  timer += DT;
  if (timer >= spawn_time) {
    timer = 0.0f;

    SDL_FPoint p = camera->rand_point_in_view();
    add(EnemyType::Base, p.x, p.y);
    count += 1;
  }

  // physics process
  for (int i = 0; i < count; i++) {

    // main physics
    SDL_FPoint attraction_vec = {anchor.x - x_curr[i], anchor.y - y_curr[i]};
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

    // collisions with eachother
    for (int j = 0; j < count; j++) {
      if (i == j)
        continue;

      // check if points are touching
      SDL_FPoint vec = {x_curr[i] - x_curr[j], y_curr[i] - y_curr[j]};
      float dist = sqrt(vec.x * vec.x + vec.y * vec.y);
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

void EnemySystem::draw(SDL_Renderer *renderer, Camera *camera) {
  for (int i = 0; i < count; i++) {
    SDL_FPoint screen_pos = camera->worldToScreen({x_curr[i], y_curr[i]});
    draw_circle(renderer, screen_pos.x, screen_pos.y, radius[i]);
  }
}
