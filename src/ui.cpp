#include "ui.h"
#include "SDL3/SDL_surface.h"
#include "globals.h"

#include <system_error>

UI::UI() {
  if (!TTF_Init()) {
    throw std::runtime_error(std::string("TTF_Init failed: ") + SDL_GetError());
  }

  font = TTF_OpenFont("assets/fonts/MomoTrustDisplay-Regular.ttf", 24);
  if (!font) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
  }
}

UI::~UI() {
  if (font)
    TTF_CloseFont(font);
}

void UI::draw(SDL_Renderer *renderer) {

  // -------- Render GameState number (example: gGS.score) --------
  int altitude = gGS.altitude / 50; // whatever number you want to show
  std::string alt_text = std::to_string(altitude) + " m";

  SDL_Color white = {255, 255, 255, 255};
  SDL_Surface *text = TTF_RenderText_Blended(font, alt_text.c_str(), 0, white);
  SDL_Texture *texture = nullptr;

  if (text) {
    texture = SDL_CreateTextureFromSurface(renderer, text);
    SDL_DestroySurface(text);
  } else {
    throw std::runtime_error("Failed to create text surface");
  }
  if (!texture) {
    SDL_Log("Couldn't create text: %s\n", SDL_GetError());
  }

  int textW = texture->w;
  int textH = texture->h;

  SDL_FRect dst = {(float)gGS.winW - textW - 10.0f, // right margin
                   10.0f,                           // top margin
                   (float)textW, (float)textH};

  SDL_RenderTexture(renderer, texture, nullptr, &dst);
  SDL_DestroyTexture(texture);
}
