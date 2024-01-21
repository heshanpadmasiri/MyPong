#include "entity.h"
#include "raylib.h"
#include <cstddef>
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 450

#define BAT_WIDTH 200
#define BAT_HEIGHT 20
#define BAT_LINE 400

#define BALL_RADIUS 15
#define BALL_COLOR RED

#define BAT_COLOR BLACK
#define TARGET_FRAME_RATE 60

#define FRAME_THICKNESS 10
#define FRAME_PART_WEIGHT 1000
#define FRAME_COLOR MAGENTA

void addFrame(std::vector<Entity *> *entities) {
  Rectangle frameBoxes[] = {
      {0, 0, WINDOW_WIDTH, FRAME_THICKNESS}, // top
      {0, FRAME_THICKNESS, FRAME_THICKNESS,
       WINDOW_HEIGHT - FRAME_THICKNESS}, // left
      {FRAME_THICKNESS, WINDOW_HEIGHT - FRAME_THICKNESS,
       WINDOW_WIDTH - FRAME_THICKNESS, FRAME_THICKNESS}, // bottom
      {WINDOW_WIDTH - FRAME_THICKNESS, FRAME_THICKNESS, FRAME_THICKNESS,
       WINDOW_WIDTH - 2 * FRAME_THICKNESS} // right
  };
  for (int i = 0; i < 4; i++) {
    Rectangle rectangle = frameBoxes[i];
    FramePart *part =
        new FramePart(rectangle, FRAME_PART_WEIGHT, FRAME_COLOR, 10 + i);
    entities->push_back(part);
  }
}

void handleCollisionChecks(std::vector<Entity *> *entities) {
  for (size_t i = 0; i < entities->size(); i++) {
    for (size_t j = i + i; j < entities->size(); j++) {
      Entity *e1 = entities->at(i);
      Entity *e2 = entities->at(j);
      if (e1->couldSkipCollisionCheck() && e2->couldSkipCollisionCheck()) {
        continue;
      }
      if (isColliding(e1, e2)) {
        resolveCollision(e2, e1);
      }
    }
  }
}

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
  addFrame(&entities); // NOTE: frame id's start form 10

  SetTargetFPS(TARGET_FRAME_RATE);
  int collisionDebounce = 0;
  while (!WindowShouldClose()) {
    if (collisionDebounce == 0) {
      // TODO: factor out to seperate function
      handleCollisionChecks(&entities);
      collisionDebounce =
          10; // NOTE: this is to avoid doing collision checks every frame
              // (instead we'll do it 6 times a second)
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
