#include "rope.h"
#include "SDL3/SDL_rect.h"
#include "globals.h"
#include "utils.h"

#include <algorithm>

Rope::Rope() {
  x_curr.reserve(NUM_POINTS);
  y_curr.reserve(NUM_POINTS);
  x_prev.reserve(NUM_POINTS);
  y_prev.reserve(NUM_POINTS);
  screen_points.reserve(NUM_POINTS);
  masses.reserve(NUM_POINTS);

  for (int i = 0; i < NUM_POINTS; ++i) {
    x_curr[i] = gGS.winW / 2.0f - (NUM_POINTS / 2.0f * POINT_SPACING) +
                i * POINT_SPACING;
    y_curr[i] = gGS.winH / 2.0f;
    x_prev[i] = x_curr[i];
    y_prev[i] = y_curr[i];
    screen_points[i] = {x_curr[i], y_curr[i]};
    if (i == NUM_POINTS - 1)
      masses[i] = 10.0f; // Last point is the ball
    else
      masses[i] = 0.0005f; // Other points are lighter
  }
}

Rope::~Rope() {}

SDL_FPoint Rope::get_end() {
  return {x_curr[NUM_POINTS - 1], y_curr[NUM_POINTS - 1]};
}

SDL_FPoint Rope::get_anchor() { return {x_curr[0], y_curr[0]}; }

float Rope::get_altitude() {
  SDL_FPoint midpoint = (get_end() + get_anchor()) / 2.0f;
  float altitude = gGS.winH - midpoint.y - FLOOR_HEIGHT;
  return altitude / 50.0f;
}

float Rope::get_speed() {
  // SDL_FPoint vel = points[NUM_POINTS - 1] - prevPoints[NUM_POINTS - 1];
  // float speed = magnitude(vel) / DT;
  // return speed;
  float raw = end_speed;
  static float filtered = 0.0f;

  float alpha = 0.1f; // smoothing factor, 0.05–0.3 works well
  filtered = filtered + alpha * (raw - filtered);
  // filtered /= 50.0f;

  return (filtered > 0.4f ? filtered : 0.0f);
}

void Rope::solve_collisions(float &y) {
  if (y >= gGS.winH - FLOOR_HEIGHT) {
    float diff = y - (gGS.winH - FLOOR_HEIGHT);
    y -= diff;
  }
}

vector<float> &Rope::get_x() { return x_curr; }
vector<float> &Rope::get_y() { return y_curr; }

void Rope::solve_physics() {
  SDL_FPoint G = {0.0f, GRAVITY};

  for (int i = (gGS.isDragging ? 1 : 0); i < NUM_POINTS; ++i) {

    SDL_FPoint f = masses[i] * G;

    // --- air drag ---
    SDL_FPoint vel = {x_curr[i] - x_prev[i], y_curr[i] - y_prev[i]};
    if (i == NUM_POINTS - 1)
      end_speed = magnitude(vel); // / DT;

    float vMag = sqrtf(vel.x * vel.x + vel.y * vel.y);
    if (vMag > 1e-4f) {
      SDL_FPoint vDir = {vel.x / vMag, vel.y / vMag};
      float dragCoeff = gGS.isDragging ? AIR_RESISTANCE * 0.8f : AIR_RESISTANCE;
      f += -dragCoeff * vMag * vDir;
    }

    // --- floor friction (horizontal only) ---
    if (y_curr[i] >= gGS.winH - FLOOR_HEIGHT) {
      // Only apply if moving horizontally
      float vx = vel.x;
      if (fabsf(vx) > 1e-5f) {
        // Oppose motion, proportional to sign of vx
        float ff = FLOOR_FRICTION;
        float frictionForce = -ff * masses[i] * (vx > 0 ? 1.0f : -1.0f);
        f.x += frictionForce;
      }
    }

    // --- integrate ---
    vel *= DAMPING;
    SDL_FPoint a = f / masses[i];

    float x_new = x_curr[i] + vel.x + DT * DT * a.x;
    float y_new = y_curr[i] + vel.y + DT * DT * a.y;
    x_prev[i] = x_curr[i];
    y_prev[i] = y_curr[i];
    x_curr[i] = x_new;
    y_curr[i] = y_new;

    solve_collisions(y_curr[i]);
  }
}

void Rope::forward_constraints() {
  for (int i = 0; i < NUM_POINTS - 1; ++i) {
    float &x1 = x_curr[i];
    float &y1 = y_curr[i];
    float &x2 = x_curr[i + 1];
    float &y2 = y_curr[i + 1];

    float dx = x2 - x1;
    float dy = y2 - y1;
    float dist = sqrtf(dx * dx + dy * dy);
    float diff = (dist - POINT_SPACING) / dist;

    float offsetX = dx * 0.5f * diff;
    float offsetY = dy * 0.5f * diff;

    // Don't move the dragged point
    if (i != 0) {
      x1 += offsetX;
      y1 += offsetY;
    }
    x2 -= offsetX;
    y2 -= offsetY;
  }
}

void Rope::backward_constraints() {
  for (int i = NUM_POINTS - 2; i >= 0; --i) {
    float &x1 = x_curr[i];
    float &y1 = y_curr[i];
    float &x2 = x_curr[i + 1];
    float &y2 = y_curr[i + 1];

    float dx = x2 - x1;
    float dy = y2 - y1;
    float dist = sqrtf(dx * dx + dy * dy);
    float diff = (dist - POINT_SPACING) / dist;

    float offsetX = dx * 0.5f * diff;
    float offsetY = dy * 0.5f * diff;

    // Don't move the fixed end ball
    if (i + 1 == NUM_POINTS - 1) {
      float ball_move_fraction = 0.005f; // small fraction of constraint offset
      x1 -= offsetX * ball_move_fraction;
      y1 -= offsetY * ball_move_fraction;
    } else {
      x2 -= offsetX;
      y2 -= offsetY;
    }
    x1 += offsetX;
    y1 += offsetY;
  }
}

void Rope::solve_constraints() {
  for (int iter = 0; iter < CONSTRAINT_ITERATIONS; ++iter) {
    if (gGS.isDragging) {
      // iterate forwards
      forward_constraints();
    } else {
      // iterate backwards
      backward_constraints();
    }
  }
}

void Rope::update(SDL_FPoint mousePos) {

  // First point follows the target

  if (gGS.isDragging) {
    if (!anchored) {
      x_curr[0] += 0.2 * (mousePos.x - x_curr[0]);
      y_curr[0] += 0.2 * (mousePos.y - y_curr[0]);
      if (point_distance({x_curr[0], y_curr[0]}, mousePos) < 4.0f)
        anchored = true;
    } else {
      x_curr[0] = mousePos.x;
      y_curr[0] = mousePos.y;
      x_prev[0] = x_curr[0];
      y_prev[0] = y_curr[0];
    }
  } else {
    anchored = false;
  }

  // Apply forces
  solve_physics();

  // Enforce constraints
  solve_constraints();
}

void Rope::draw(SDL_Renderer *renderer, Camera &camera) {
  for (int i = 0; i < NUM_POINTS; i++) {
    screen_points[i] = camera.worldToScreen({x_curr[i], y_curr[i]});
  }

  float ropeY = get_end().y;
  float space_y = -(10000.0 - gGS.winH);
  float transition_y = space_y + 800.0f;
  if (ropeY <= transition_y) {

    float clamped = std::max(ropeY, space_y);

    // Map linearly from 800 → 950 to 0 → 255
    brightness =
        (int)((transition_y - clamped) / (transition_y - space_y) * 255.0f);
  }
  SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
  // SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);

  SDL_RenderLines(renderer, screen_points.data(), NUM_POINTS);
  draw_circle(renderer, static_cast<int>(screen_points[NUM_POINTS - 1].x),
              static_cast<int>(screen_points[NUM_POINTS - 1].y),
              static_cast<int>(BALL_RADIUS));
}

// Experiments with mass-aware constraints and additional backward constraints
//--------------------------------
// void Rope::forward_constraints() {
//   // --- Forward pass with mass weighting ---
//   for (int i = 0; i < NUM_POINTS - 1; ++i) {
//     SDL_FPoint &p1 = points[i];
//     SDL_FPoint &p2 = points[i + 1];
//
//     float dx = p2.x - p1.x;
//     float dy = p2.y - p1.y;
//     float dist = sqrtf(dx * dx + dy * dy);
//     float diff = (dist - POINT_SPACING) / dist;
//
//     // Mass-aware offsets
//     float m1 = masses[i];
//     float m2 = masses[i + 1];
//     float sum = m1 + m2;
//
//     float ratio1 = m2 / sum; // p1 moves more if p2 is heavy
//     float ratio2 = m1 / sum; // p2 moves more if p1 is light
//     // Soften the ratios by blending with 0.5 (equal split)
//     float blend = 0.8f; // 0 = full 0.5, 1 = full mass weighting
//     ratio1 = 0.5f * (1.0f - blend) + ratio1 * blend;
//     ratio2 = 0.5f * (1.0f - blend) + ratio2 * blend;
//
//     float offsetX = dx * 0.5f * diff;
//     float offsetY = dy * 0.5f * diff;
//
//     float max_disp = POINT_SPACING * 0.5f;
//     float disp_mag = sqrt(offsetX * offsetX + offsetY * offsetY);
//     if (disp_mag > max_disp) {
//       float scale = max_disp / disp_mag;
//       offsetX *= scale;
//       offsetY *= scale;
//     }
//
//     if (!(i == 0)) { // dragged point stays fixed
//       p1.x += offsetX * ratio1;
//       p1.y += offsetY * ratio1;
//     }
//
//     p2.x -= offsetX * ratio2;
//     p2.y -= offsetY * ratio2;
//   }

// --- Tiny backward pass on the last few segments ---
// int N_back = 7; // last N segments to correct, tune as needed
// for (int i = NUM_POINTS - 2; i >= NUM_POINTS - 1 - N_back; --i) {
//   SDL_FPoint &p1 = points[i];
//   SDL_FPoint &p2 = points[i + 1];
//
//   float dx = p2.x - p1.x;
//   float dy = p2.y - p1.y;
//   float dist = sqrtf(dx * dx + dy * dy);
//   float diff = (dist - POINT_SPACING) / dist;
//
//   float offsetX = dx * 0.5f * diff;
//   float offsetY = dy * 0.5f * diff;
//
//   // Only move p1 slightly; keep p2 (heavy ball) fully free
//   float back_fraction = 0.7f; // small fraction
//   p1.x += offsetX * back_fraction;
//   p1.y += offsetY * back_fraction;
//   p2.x -= offsetX * back_fraction;
//   p2.y -= offsetY * back_fraction;
// }
//}
