#include "world.h"
#include "globals.h"

Background::Background(SDL_Renderer *renderer) {
  for (int i = 0; i < 6; ++i) {
    BGLayer layer;
    SDL_Surface *surface = SDL_LoadPNG(layerFiles[i]);
    layer.texture = SDL_CreateTextureFromSurface(renderer, surface);
    layer.scrollSpeed = scrollSpeeds[i];
    layer.offset = 0.0f;
    layers[i] = layer;
  }
}

Background::~Background() {
  for (auto &layer : layers)
    SDL_DestroyTexture(layer.texture);
}

void Background::draw(SDL_Renderer *renderer, Camera *camera) {
  for (auto &layer : layers) {
    float texW, texH;
    SDL_GetTextureSize(layer.texture, &texW, &texH);

    // Scale to fill screen
    float scale =
        fmax((float)gGameState.winW / texW, (float)gGameState.winH / texH);
    float drawW = texW * scale;
    float drawH = texH * scale;

    // Scroll offset in *screen space*
    float cameraX = camera->get_pos().x;
    float layerScroll = layer.scrollSpeed * 0.5 * cameraX;

    // Wrap around so the layer repeats seamlessly
    layer.offset = fmod(layerScroll, drawW);
    if (layer.offset < 0)
      layer.offset += drawW; // handle negative positions

    // Full texture src
    SDL_FRect src = {0, 0, (float)texW, (float)texH};

    // Draw first tile
    SDL_FRect dest1 = {-layer.offset, 0, drawW, drawH};
    SDL_RenderTexture(renderer, layer.texture, &src, &dest1);

    // Draw second tile for seamless wrap
    SDL_FRect dest2 = {drawW - layer.offset, 0, drawW, drawH};
    SDL_RenderTexture(renderer, layer.texture, &src, &dest2);
  }
}
