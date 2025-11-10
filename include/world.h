#pragma once

#include <SDL3/SDL.h>
#include <cmath>

#include "camera.h"

struct BGLayer {
  SDL_Texture *texture;
  float scrollSpeed;
  float offsetX; // scroll offset
  float offsetY; // scroll offset
};

class Background {
  const char *layerFiles[6] = {
      "assets/background/layer0.png", "assets/background/layer1.png",
      "assets/background/layer2.png", "assets/background/layer3.png",
      "assets/background/layer4.png", "assets/background/layer5.png",
  };

  const float scrollSpeeds[6] = {0.06f, 0.2f, 0.4f, 0.65f, 1.0f, 1.33f};
  BGLayer layers[6];

public:
  bool load(SDL_Renderer *renderer);

  Background(SDL_Renderer *renderer);

  ~Background();

  void draw(SDL_Renderer *renderer, Camera *camera);
};
