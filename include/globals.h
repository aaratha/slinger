#pragma once

typedef struct {
  int winW, winH;
  bool isDragging;
  int altitude;
  float speed;
} GameState;

extern GameState gGS;
