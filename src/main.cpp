#include <SDL3/SDL.h>
#include <cmath>

#define NUM_POINTS 20
#define BALL_RADIUS 10.0f
#define POINT_SPACING 10.0f
#define GRAVITY 3000.0f
#define DT 0.016f
#define CONSTRAINT_ITERATIONS 8
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define DAMPING 0.99f

SDL_FPoint operator*(float scalar, const SDL_FPoint &point) {
  return SDL_FPoint{scalar * point.x, scalar * point.y};
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

class Rope {
  SDL_FPoint points[NUM_POINTS];
  SDL_FPoint prevPoints[NUM_POINTS];

public:
  Rope() {
    for (int i = 0; i < NUM_POINTS; ++i) {
      points[i] = {WINDOW_WIDTH / 2.0f - (NUM_POINTS / 2.0f * POINT_SPACING) +
                       i * POINT_SPACING,
                   WINDOW_HEIGHT / 2.0f};
      prevPoints[i] = points[i];
    }
  }

  void update(float targetX, float targetY) {
    // First point follows the target
    points[0].x = targetX;
    points[0].y = targetY;

    // Apply gravity
    for (int i = 1; i < NUM_POINTS; ++i) {
      SDL_FPoint temp = points[i];
      points[i] += DAMPING * (points[i] - prevPoints[i]) +
                   GRAVITY * DT * DT * SDL_FPoint{0.0f, 1.0f};
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

        if (i != 0) {
          p1.x += offsetX;
          p1.y += offsetY;
        }
        p2.x -= offsetX;
        p2.y -= offsetY;
      }
    }
  }

  void draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);

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
      }
    }

    rope.update(static_cast<float>(mouseX), static_cast<float>(mouseY));

    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);

    rope.draw(renderer);

    SDL_RenderPresent(renderer);

    SDL_Delay(16); // ~60fps
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
