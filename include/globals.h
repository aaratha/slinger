#pragma once

typedef struct {
  int winW, winH;
  bool isDragging;
  int altitude;
  float speed;
  float enemy_radius;
} GameState;

extern GameState gGS;
