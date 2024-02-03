#include "engine.h"
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

#define PRESSURE_SCALE 100.0f

void addFrame(Engine *engine) {
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
    engine->addEntity(part);
  }
}

// void handleCollisionChecks(std::vector<Entity *> *entities) {
//   for (size_t i = 0; i < entities->size(); i++) {
//     for (size_t j = i + 1; j < entities->size(); j++) {
//       Entity *e1 = entities->at(i);
//       Entity *e2 = entities->at(j);
//       if (e1->couldSkipCollisionCheck() && e2->couldSkipCollisionCheck()) {
//         continue;
//       }
//       if (isColliding(e1, e2)) {
//         resolveCollision(e2, e1);
//       }
//     }
//   }
// }

// void handleUserInputs(Bat *bat) {
//   MotionForce *motionForce = nullptr;
//   if (IsKeyDown(KEY_H)) {
//     motionForce = new MotionForce(PRESSURE_SCALE, LEFT);
//   } else if (IsKeyDown(KEY_L)) {
//     motionForce = new MotionForce(PRESSURE_SCALE, RIGHT);
//   } else if (IsKeyDown(KEY_J)) {
//     motionForce = new MotionForce(PRESSURE_SCALE, DOWN);
//   } else if (IsKeyDown(KEY_K)) {
//     motionForce = new MotionForce(PRESSURE_SCALE, UP);
//   }

//   if (motionForce != nullptr) {
//     bat->applyNextFrame(motionForce);
//   }
// }
void handleBatInputs(Entity *bat) {
  MotionForce *motionForce = nullptr;
  if (IsKeyDown(KEY_H)) {
    motionForce = new MotionForce(PRESSURE_SCALE, LEFT);
  } else if (IsKeyDown(KEY_L)) {
    motionForce = new MotionForce(PRESSURE_SCALE, RIGHT);
  } else if (IsKeyDown(KEY_J)) {
    motionForce = new MotionForce(PRESSURE_SCALE, DOWN);
  } else if (IsKeyDown(KEY_K)) {
    motionForce = new MotionForce(PRESSURE_SCALE, UP);
  }

  if (motionForce != nullptr) {
    bat->applyNextFrame(motionForce);
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My pong");
  float centerX = GetScreenWidth() / 2;

  Ball *ball = new Ball({centerX, 50}, 100, RED, 15);
  ball->applyContiniously(new Gravity());
  Bat *bat = new Bat({centerX - BAT_WIDTH / 2, BAT_LINE}, 100, BAT_COLOR,
                     BAT_WIDTH, BAT_HEIGHT);


  SetTargetFPS(TARGET_FRAME_RATE);
  Engine *engine = new Engine();
  engine->addEntity(ball);
  engine->addEntity(bat);
  engine->setInputHandler(bat, &handleBatInputs);
  addFrame(engine); // NOTE: frame id's start form 10

  while (!WindowShouldClose()) {
    engine->renderFrame();
  }
  delete engine;
  CloseWindow();
  return 0;
}
