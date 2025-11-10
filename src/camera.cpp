#include "camera.h"
#include "utils.h"

Camera::Camera() { pos = {0.0f, 0.0f}; }
Camera::~Camera() {}

SDL_FPoint Camera::worldToScreen(const SDL_FPoint &world, int winW, int winH) {
  return {world.x - pos.x + winW / 2.0f, world.y - pos.y + winH / 2.0f};
}

SDL_FPoint Camera::screenToWorld(const SDL_FPoint &screen, int winW,
                                 int winH) const {
  return {screen.x + pos.x - winW / 2.0f, screen.y + pos.y - winH / 2.0f};
}

SDL_FPoint Camera::get_pos() { return pos; }

void Camera::update(SDL_FPoint anchor, SDL_FPoint end) {
  SDL_FPoint target = (anchor + end) / 2;
  pos = lerp(pos, target, 0.1);
}
