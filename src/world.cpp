#include "world.h"
#include "globals.h"

bool Background::load(SDL_Renderer *renderer) {
  for (int i = 0; i < 6; ++i) {
    BGLayer layer;

    SDL_Surface *surface = SDL_LoadPNG(layerFiles[i]);
    if (!surface) {
      SDL_Log("Failed to load %s", layerFiles[i]);
      return false;
    }

    layer.texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!layer.texture) {
      SDL_Log("Failed to create texture for %s: %s", layerFiles[i],
              SDL_GetError());
      return false;
    }

    layer.scrollSpeed = scrollSpeeds[i];
    layer.offsetX = 0.0f;
    layer.offsetY = 0.0f;
    layers[i] = layer;

    float w, h;
    SDL_GetTextureSize(layer.texture, &w, &h);
    SDL_Log("Loaded %s: (%f x %f)", layerFiles[i], w, h);
  }
  return true;
}

Background::Background(SDL_Renderer *renderer) {
  bool result = load(renderer);
  if (result)
    SDL_Log("Background layers loaded!");
}

Background::~Background() {
  for (auto &layer : layers)
    SDL_DestroyTexture(layer.texture);
}

void Background::draw(SDL_Renderer *renderer, Camera *camera) {
  for (auto &layer : layers) {
    float texW, texH;
    SDL_GetTextureSize(layer.texture, &texW, &texH);

    float scale = static_cast<float>(gGameState.winW) / texW;
    float drawW = texW * scale;
    float drawH = texH * scale;

    SDL_FPoint camera_pos = camera->get_pos();

    float scrollFactor = layer.scrollSpeed;

    // Horizontal parallax
    layer.offsetX = fmodf(scrollFactor * camera_pos.x, drawW);
    if (layer.offsetX < 0)
      layer.offsetX += drawW;

    // Vertical parallax: adjust relative to camera's minimum y
    float camMinY = gGameState.winH / 2.0f;
    layer.offsetY = scrollFactor * (camera_pos.y - camMinY);

    // Base position aligns bottom edge when camera at minY
    float baseY = gGameState.winH - drawH;
    float drawY = baseY - layer.offsetY;

    SDL_FRect src = {0, 0, texW, texH};

    SDL_FRect dests[4] = {
        {-layer.offsetX, drawY, drawW, drawH},
        {drawW - layer.offsetX, drawY, drawW, drawH},
        {-layer.offsetX, drawY + drawH, drawW, drawH},
        {drawW - layer.offsetX, drawY + drawH, drawW, drawH},
    };

    for (auto &dest : dests)
      SDL_RenderTexture(renderer, layer.texture, &src, &dest);
  }
}
