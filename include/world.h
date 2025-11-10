#pragma once

#include <SDL3/SDL.h>
#include <cmath>

#include "camera.h"

struct BGLayer {
  SDL_Texture *texture;
  float scrollSpeed;
  float offset; // scroll offset
};

class Background {
  const char *layerFiles[6] = {
      "assets/background/layer0.png", "assets/background/layer1.png",
      "assets/background/layer2.png", "assets/background/layer3.png",
      "assets/background/layer4.png", "assets/background/layer5.png",
  };

  const float scrollSpeeds[6] = {0.1f, 0.3f, 0.6f, 1.0f, 1.5f, 2.0f};
  BGLayer layers[6];

public:
  Background(SDL_Renderer *renderer);

  ~Background();

  void draw(SDL_Renderer *renderer, Camera *camera);
};
