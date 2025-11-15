#pragma once

typedef struct {
  int winW, winH;
  bool isDragging;
  int altitude;
} GameState;

extern GameState gGS;
