#include <SDL3/SDL.h>
#include <cmath>

#define NUM_POINTS 20
#define BALL_RADIUS 10.0f
#define POINT_SPACING 10.0f
#define GRAVITY 1000.0f
#define DT 0.016f
#define CONSTRAINT_ITERATIONS 8
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define DAMPING 0.99f
#define AIR_RESISTANCE 0.001f

SDL_FPoint operator*(float scalar, const SDL_FPoint &point) {
  return SDL_FPoint{scalar * point.x, scalar * point.y};
}

SDL_FPoint operator/(const SDL_FPoint &point, float scalar) {
  return SDL_FPoint{point.x / scalar, point.y / scalar};
}

SDL_FPoint operator+(const SDL_FPoint &a, const SDL_FPoint &b) {
  return SDL_FPoint{a.x + b.x, a.y + b.y};
}

SDL_FPoint operator-(const SDL_FPoint &a, const SDL_FPoint &b) {
  return SDL_FPoint{a.x - b.x, a.y - b.y};
}

SDL_FPoint &operator+=(SDL_FPoint &a, const SDL_FPoint &b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

float point_distance(const SDL_FPoint &a, const SDL_FPoint &b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return sqrtf(dx * dx + dy * dy);
}

void draw_circle(SDL_Renderer *renderer, int32_t centerX, int32_t centerY,
                 int32_t radius) {
  for (int w = 0; w < radius * 2; w++) {
    for (int h = 0; h < radius * 2; h++) {
      int dx = radius - w; // horizontal offset
      int dy = radius - h; // vertical offset
      if ((dx * dx + dy * dy) <= (radius * radius)) {
        SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
      }
    }
  }
}
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
  Background(SDL_Renderer *renderer) {
    for (int i = 0; i < 6; ++i) {
      BGLayer layer;
      SDL_Surface *surface = SDL_LoadPNG(layerFiles[i]);
      layer.texture = SDL_CreateTextureFromSurface(renderer, surface);
      layer.scrollSpeed = scrollSpeeds[i];
      layer.offset = 0.0f;
      layers[i] = layer;
    }
  }

  ~Background() {
    for (auto &layer : layers)
      SDL_DestroyTexture(layer.texture);
  }

  void draw(SDL_Renderer *renderer) {
    int winW, winH;
    SDL_GetRenderOutputSize(renderer, &winW, &winH);

    for (auto &layer : layers) {
      float texW, texH;
      SDL_GetTextureSize(layer.texture, &texW, &texH);

      // Scale to fill screen
      float scale = fmax((float)winW / texW, (float)winH / texH);
      float drawW = texW * scale;
      float drawH = texH * scale;

      // Scroll offset in *screen space*
      layer.offset += layer.scrollSpeed;
      if (layer.offset > drawW)
        layer.offset -= drawW;

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
};

class Rope {
  SDL_FPoint points[NUM_POINTS];
  SDL_FPoint prevPoints[NUM_POINTS];
  float masses[NUM_POINTS];
  bool anchored = false;

public:
  Rope() {
    for (int i = 0; i < NUM_POINTS; ++i) {
      points[i] = {WINDOW_WIDTH / 2.0f - (NUM_POINTS / 2.0f * POINT_SPACING) +
                       i * POINT_SPACING,
                   WINDOW_HEIGHT / 2.0f};
      prevPoints[i] = points[i];
      if (i == NUM_POINTS - 1)
        masses[i] = 1.0f; // Last point is the ball
      else
        masses[i] = 0.1f; // Other points are lighter
    }
  }

  void update(SDL_FPoint mousePos, bool isDragging) {
    // First point follows the target
    SDL_FPoint G = {0.0f, GRAVITY};

    if (isDragging) {
      if (!anchored) {
        points[0] += 0.2 * (mousePos - points[0]);
        if (point_distance(points[0], mousePos) < 4.0f)
          anchored = true;
      } else {
        points[0] = mousePos;
        prevPoints[0] = points[0]; // <-- resets motion to zero
      }
    } else {
      anchored = false;
    }

    // Apply forces
    for (int i = (isDragging ? 1 : 0); i < NUM_POINTS; ++i) {
      SDL_FPoint v = (points[i] - prevPoints[i]) / DT;
      float v_mag = sqrtf(v.x * v.x + v.y * v.y);
      SDL_FPoint v_dir = v_mag > 0 ? v / v_mag : SDL_FPoint{0, 0};

      SDL_FPoint f_drag = -AIR_RESISTANCE * v_mag * v_dir;
      SDL_FPoint f = masses[i] * G + f_drag;

      SDL_FPoint a = f / masses[i];

      SDL_FPoint temp = points[i];
      points[i] += DAMPING * (points[i] - prevPoints[i]) + DT * DT * a;
      prevPoints[i] = temp;
    }

    // Enforce constraints
    for (int iter = 0; iter < CONSTRAINT_ITERATIONS; ++iter) {
      for (int i = 0; i < NUM_POINTS - 1; ++i) {
        SDL_FPoint &p1 = points[i];
        SDL_FPoint &p2 = points[i + 1];

        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float dist = sqrtf(dx * dx + dy * dy);
        float diff = (dist - POINT_SPACING) / dist;

        float offsetX = dx * 0.5f * diff;
        float offsetY = dy * 0.5f * diff;

        if (!(isDragging && i == 0)) {
          p1.x += offsetX;
          p1.y += offsetY;
        }
        p2.x -= offsetX;
        p2.y -= offsetY;
      }
    }
  }

  void draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);

    SDL_RenderLines(renderer, points, NUM_POINTS);
    draw_circle(renderer, static_cast<int>(points[NUM_POINTS - 1].x),
                static_cast<int>(points[NUM_POINTS - 1].y),
                static_cast<int>(BALL_RADIUS));
  }
};

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

  bool running = true;
  int mouseX = 0, mouseY = 0;
  bool isDragging = false;

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
          isDragging = true;
        }
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          isDragging = false;
        }
      }
    }

    SDL_FPoint mousePos = {static_cast<float>(mouseX),
                           static_cast<float>(mouseY)};
    rope.update(mousePos, isDragging);

    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);

    bg.draw(renderer);
    rope.draw(renderer);

    SDL_RenderPresent(renderer);

    SDL_Delay(16); // ~60fps
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
