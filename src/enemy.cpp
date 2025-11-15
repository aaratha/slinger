#include "enemy.h"

#include "globals.h"
#include "utils.h"

// -----------Enemy-----------

Enemy::Enemy(SDL_FPoint p) {
  pos = p;
  prev_pos = p;
  attraction = 1000.0f;
  radius = 10.0f;
  mass = 1.0f;
}

Enemy::~Enemy() {}

float Enemy::get_attraction() { return attraction; }

void Enemy::update(SDL_FPoint anchor) {
  // main physics
  SDL_FPoint attraction_vec = anchor - pos;
  float attraction_vec_mag = magnitude(attraction_vec);
  SDL_FPoint attraction_dir = attraction_vec / attraction_vec_mag;

  SDL_FPoint f = get_attraction() * attraction_dir;

  SDL_FPoint vel = pos - prev_pos;
  vel *= DAMPING;
  SDL_FPoint f_drag = -AIR_RESISTANCE * vel;
  f += f_drag;

  SDL_FPoint accel = f / mass;

  SDL_FPoint new_pos = pos + vel + DT * DT * accel;
  prev_pos = pos;
  pos = new_pos;

  // collisions
  if (pos.y >= gGS.winH - FLOOR_HEIGHT) {
    float diff = pos.y - (gGS.winH - FLOOR_HEIGHT);
    pos.y -= diff;
  }
}

void Enemy::draw(SDL_Renderer *renderer, Camera *camera) {
  SDL_FPoint screen_pos = camera->worldToScreen(pos);
  draw_circle(renderer, screen_pos.x, screen_pos.y, radius);
}

// -----------Enemies-----------

Enemies::Enemies() {
  timer = 0.0f;
  spawn_time = 2.0f;
}

Enemies::~Enemies() {}

void Enemies::update(Camera *camera, SDL_FPoint anchor) {
  // spawn process
  timer += DT;
  if (timer >= spawn_time) {
    timer = 0.0f;
    SDL_FPoint p = camera->rand_point_in_view();
    enemies.push_back(make_unique<Enemy>(p));
  }

  // physics process
  for (auto &e : enemies) {
    e->update(anchor);
  }
}

void Enemies::draw(SDL_Renderer *renderer, Camera *camera) {
  for (auto &e : enemies) {
    e->draw(renderer, camera);
  }
}
