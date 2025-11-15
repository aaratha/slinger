#pragma once

#include "SDL3/SDL_render.h"
#include <SDL3_ttf/SDL_ttf.h>

class UI {
  TTF_Font *font;

public:
  UI();
  ~UI();
  void draw(SDL_Renderer *renderer);
};
