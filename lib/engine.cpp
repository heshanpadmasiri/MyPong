#include "engine.h"
#include "entity.h"
#include "raylib.h"
#include <utility>
#include <vector>

void handleCollisions(std::vector<Entity *> entities) {
  for (size_t i = 0; i < entities.size(); i++) {
    for (size_t j = i + 1; j < entities.size(); j++) {
      Entity *e1 = entities.at(i);
      Entity *e2 = entities.at(j);
      if (e1->couldSkipCollisionCheck() && e2->couldSkipCollisionCheck()) {
        continue;
      }
      if (isColliding(e1, e2)) {
        resolveCollision(
            e2, e1); // TODO: maybe move the collision logic here as well
      }
    }
  }
}

void renderFrame(GameState *state) {
  for (auto handlerPair : state->inputHandlers) {
    handlerPair->second(handlerPair->first);
  }
  handleCollisions(state->entities);
  for (Entity *entity : state->entities) {
    entity->update(GetFrameTime());
  }
  // TODO: handle collision
  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (Entity *entity : state->entities) {
    entity->draw();
  }
  EndDrawing();
}
