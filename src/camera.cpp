#include "camera.h"
#include "globals.h"
#include "utils.h"

#include <algorithm>

Camera::Camera() {
  pos = {0.0f, 0.0f};
  speed = 0.0f;
}
Camera::~Camera() {}

SDL_FPoint Camera::worldToScreen(const SDL_FPoint &world) {
  return {world.x - pos.x + gGS.winW / 2.0f, world.y - pos.y + gGS.winH / 2.0f};
}

SDL_FPoint Camera::screenToWorld(const SDL_FPoint &screen) const {
  return {screen.x + pos.x - gGS.winW / 2.0f,
          screen.y + pos.y - gGS.winH / 2.0f};
}

SDL_FPoint Camera::get_pos() { return pos; }

SDL_FPoint Camera::rand_point_in_view() {
  float randX = SDL_randf();
  float randY = SDL_randf();

  float screenX = randX * gGS.winW;
  float screenY = randY * gGS.winH;

  const SDL_FPoint screen_p = {screenX, screenY};
  SDL_FPoint p = screenToWorld(screen_p);

  return p;
}

void Camera::update(SDL_FPoint anchor, SDL_FPoint end) {
  if (!gGS.isDragging)
    speed = lerp1D(speed, 0.3f, 0.3f);
  else
    speed = lerp1D(speed, 0.0f, 0.3f);

  SDL_FPoint target = (anchor + end) / 2;

  pos = lerp2D(pos, target, speed);
  pos.y = std::min(pos.y, (float)gGS.winH / 2.0f);
}
