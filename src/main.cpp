#include "raylib.h"
#include <cstdio>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 450

#define BAT_WIDTH 200
#define BAT_HEIGHT 20
#define BAT_LINE 400

#define BALL_RADIUS 15
#define BALL_COLOR RED

#define BAT_COLOR BLACK

struct GameState {
  float ballX;
  int ballY;
  float batX;
};

void drawFrame(GameState state) {
  {
    // draw bat
    float x_corner = state.batX - (BAT_WIDTH / 2);
    Rectangle rec = {x_corner, BAT_LINE, BAT_WIDTH, BAT_HEIGHT};
    DrawRectangleRec(rec, BAT_COLOR);
  }
  {
    // draw ball
    DrawCircle(state.ballX, state.ballY, BALL_RADIUS, BALL_COLOR);
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  SetTargetFPS(60);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My pong");
  float centerX = GetScreenWidth() / 2;
  GameState state { centerX, 50, centerX };
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    drawFrame(state);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
