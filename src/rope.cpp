#include "rope.h"

Rope::Rope() {
  for (int i = 0; i < NUM_POINTS; ++i) {
    points[i] = {WINDOW_WIDTH / 2.0f - (NUM_POINTS / 2.0f * POINT_SPACING) +
                     i * POINT_SPACING,
                 WINDOW_HEIGHT / 2.0f};
    prevPoints[i] = points[i];
    screenPoints[i] = points[i];
    if (i == NUM_POINTS - 1)
      masses[i] = 1.0f; // Last point is the ball
    else
      masses[i] = 0.1f; // Other points are lighter
  }
}

Rope::~Rope() {}

SDL_FPoint Rope::get_end() { return points[NUM_POINTS - 1]; }

SDL_FPoint Rope::get_anchor() { return points[0]; }

void Rope::solve_physics(bool isDragging) {
  SDL_FPoint G = {0.0f, GRAVITY};

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
}

void Rope::solve_constraints(bool isDragging) {
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

void Rope::update(SDL_FPoint mousePos, bool isDragging) {

  // First point follows the target

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
  solve_physics(isDragging);

  // Enforce constraints
  solve_constraints(isDragging);
}

void Rope::draw(SDL_Renderer *renderer, Camera *camera) {
  int winW, winH;
  SDL_GetRenderOutputSize(renderer, &winW, &winH);

  for (int i = 0; i < NUM_POINTS; i++) {
    screenPoints[i] = camera->worldToScreen(points[i], winW, winH);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  // SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);

  SDL_RenderLines(renderer, screenPoints, NUM_POINTS);
  draw_circle(renderer, static_cast<int>(screenPoints[NUM_POINTS - 1].x),
              static_cast<int>(screenPoints[NUM_POINTS - 1].y),
              static_cast<int>(BALL_RADIUS));
}
