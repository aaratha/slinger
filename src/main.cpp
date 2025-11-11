#include <SDL3/SDL.h>

#include "camera.h"
#include "globals.h"
#include "rope.h"
#include "world.h"

int main(int, char **) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *window =
      SDL_CreateWindow("Circle Follow", 800, 600, SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

  Rope rope;
  Background bg(renderer);
  Camera camera;

  gGS.winW = 800;
  gGS.winH = 600;
  gGS.isDragging = false;

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
      }
    }

    SDL_FPoint mouseScreen = {(float)mouseX, (float)mouseY};
    SDL_FPoint mouseWorld = camera.screenToWorld(mouseScreen);

    rope.update(mouseWorld);

    camera.update(rope.get_anchor(), rope.get_end());

    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);

    bg.draw(renderer, &camera);
    rope.draw(renderer, &camera);

    SDL_RenderPresent(renderer);

    SDL_Delay(16); // ~60fps
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
