#include "entity.h"
#include "raylib.h"
#include <cstddef>

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
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My pong");
  float centerX = GetScreenWidth() / 2;
  Gravity *gravity = new Gravity();

  Ball *ball = new Ball({centerX, 50}, 100, RED, 15);
  ball->applyContiniously(gravity);
  Bat *bat = new Bat({centerX - BAT_WIDTH / 2, BAT_LINE}, 100, BAT_COLOR,
                     BAT_WIDTH, BAT_HEIGHT);

  std::vector<Entity *> entities;
  entities.push_back(ball);
  entities.push_back(bat);

  SetTargetFPS(TARGET_FRAME_RATE);
  // FIXME: collision debounce logic is too simple to handle multiple entities
  int collisionDebounce = 0;
  while (!WindowShouldClose()) {
    if (collisionDebounce == 0) {
      // TODO: factor out to seperate function
      for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + i; j < entities.size(); j++) {
          Entity *e1 = entities.at(i);
          Entity *e2 = entities.at(j);
          if (isColliding(e1, e2)) {
            collisionDebounce = 10;
            resolveCollision(e2, e1);
          }
        }
      }
    } else {
      collisionDebounce -= 1;
    }
    // applyReactionForces(&entities);
    for (Entity *entity : entities) {
      entity->update(GetFrameTime());
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (Entity *entity : entities) {
      entity->draw();
    }
    EndDrawing();
  }

  CloseWindow();
  while (!entities.empty()) {
    Entity *e = entities.back();
    entities.pop_back();
    delete e;
  }
  return 0;
}
