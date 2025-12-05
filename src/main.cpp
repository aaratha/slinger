#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "SDL3/SDL_init.h"
#include "camera.h"
#include "enemy.h"
#include "globals.h"
#include "rope.h"
#include "ui.h"
#include "world.h"

int main(int, char **) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  UI ui;

  gGS.winW = 1000;
  gGS.winH = 720;
  gGS.isDragging = false;
  gGS.enemy_radius = 10.0f;
  gGS.enemy_radius = 10.0f;

  SDL_Window *window = SDL_CreateWindow("Circle Follow", gGS.winW, gGS.winH,
                                        SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

  Rope rope;
  Background bg(renderer);
  Camera camera;
  EnemySystem enemy_system;

  bool running = true;
  int mouseX = 0, mouseY = 0;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
      else if (event.type == SDL_EVENT_MOUSE_MOTION) {
        mouseX = event.motion.x;
        mouseY = event.motion.y;
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          gGS.isDragging = true;
        }
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          gGS.isDragging = false;
        }
      } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        gGS.winW = event.window.data1;
        gGS.winH = event.window.data2;
      }
    }

    SDL_FPoint mouseScreen = {(float)mouseX, (float)mouseY};
    SDL_FPoint mouseWorld = camera.screenToWorld(mouseScreen);

    rope.update(mouseWorld);
    camera.update(rope.get_anchor(), rope.get_end());
    enemy_system.update(camera, rope.get_x(), rope.get_y());

    gGS.altitude = rope.get_altitude();
    gGS.speed = rope.get_speed();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);

    bg.draw(renderer, camera);
    rope.draw(renderer, camera);
    enemy_system.draw(renderer, camera);
    ui.draw(renderer);

    SDL_RenderPresent(renderer);

    SDL_Delay(16); // ~60fps
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
