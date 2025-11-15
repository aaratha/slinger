#include "ui.h"
#include "SDL3/SDL_surface.h"
#include "globals.h"

#include <format>
#include <system_error>

UI::UI() {
  if (!TTF_Init()) {
    throw std::runtime_error(std::string("TTF_Init failed: ") + SDL_GetError());
  }

  font =
      TTF_OpenFont("assets/fonts/JetBrainsMonoNerdFontMono-ExtraBold.ttf", 24);
  if (!font) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
  }
}

UI::~UI() {
  if (font)
    TTF_CloseFont(font);
}

void UI::draw(SDL_Renderer *renderer) {
  SDL_Color white{255, 255, 255, 255};

  // ------- ALTITUDE -------
  int altitude = gGS.altitude;
  std::string alt_text = std::to_string(altitude) + " m";

  SDL_Surface *s1 = TTF_RenderText_Blended(font, alt_text.c_str(), 0, white);
  if (!s1)
    return;

  SDL_Texture *t1 = SDL_CreateTextureFromSurface(renderer, s1);
  SDL_DestroySurface(s1);

  int w1 = t1->w;
  int h1 = t1->h;

  SDL_FRect dst1{gGS.winW - w1 - 10.0f, 10.0f, (float)w1, (float)h1};

  SDL_RenderTexture(renderer, t1, nullptr, &dst1);
  SDL_DestroyTexture(t1);

  // ------- SPEED -------
  float speed = gGS.speed;
  std::string speed_text = std::format("{:.2f} m/s", speed);

  SDL_Surface *s2 = TTF_RenderText_Blended(font, speed_text.c_str(), 0, white);
  if (!s2)
    return;

  SDL_Texture *t2 = SDL_CreateTextureFromSurface(renderer, s2);
  SDL_DestroySurface(s2);

  int w2 = t2->w;
  int h2 = t2->h;

  SDL_FRect dst2{gGS.winW - w2 - 10.0f,
                 10.0f + h1 + 5.0f, // <-- put below altitude
                 (float)w2, (float)h2};

  SDL_RenderTexture(renderer, t2, nullptr, &dst2);
  SDL_DestroyTexture(t2);
}
