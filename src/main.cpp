#include "entity.h"
#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 450

#define BAT_WIDTH 200
#define BAT_HEIGHT 20
#define BAT_LINE 400

#define BALL_RADIUS 15
#define BALL_COLOR RED

#define BAT_COLOR BLACK
#define TARGET_FRAME_RATE 60

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  float centerX = GetScreenWidth() / 2;
  Gravity *gravity = new Gravity();

  Ball *ball = new Ball({centerX, 50}, 10000, RED, 15);
  ball->applyContiniously(gravity);
  Bat *bat = new Bat({centerX, BAT_LINE}, BAT_COLOR, BAT_WIDTH, BAT_HEIGHT);

  std::vector<Entity *> entities;
  entities.push_back(ball);
  entities.push_back(bat);

  SetTargetFPS(TARGET_FRAME_RATE);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My pong");
  float tickSpeed = 1.0 / TARGET_FRAME_RATE;
  while (!WindowShouldClose()) {
    // applyReactionForces(&entities);
    for (Entity *entity : entities) {
      entity->update(tickSpeed);
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (Entity *entity : entities) {
      entity->draw();
    }
    EndDrawing();
  }

  CloseWindow();
  // FIXME: deallocate objects
  return 0;
}
