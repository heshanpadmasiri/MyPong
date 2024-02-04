#include "../lib/engine.h"
#include "../lib/entity.h"
#include "../lib/raylib.h"
#include <dlfcn.h>
#include <iostream>
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

#define ENGINE "./bin/libengine.dylib"

void addWindowFrame(std::vector<Entity *> *entities) {
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

bool handleBatInputs(Entity *bat) {
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
    return true;
  }
  return false;
}

void *getLibHandle() {
  void *handle = dlopen(ENGINE, RTLD_NOW);
  if (!handle) {
    std::cerr << "Error loading library: " << dlerror() << std::endl;
    return nullptr;
  }
  return handle;
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
  std::vector<Entity *> entities;
  entities.push_back(ball);
  entities.push_back(bat);

  addWindowFrame(&entities); // NOTE: frame id's start form 10
  Engine *engine = new Engine;
  engine->addEntities(&entities);
  engine->setInputHandler(bat, &handleBatInputs);

  while (!WindowShouldClose()) {
    engine->renderFrame();
  }
  for (Entity* each : entities) {
    delete each;
  }
  delete engine;
  CloseWindow();
  return 0;
}
