#include "engine.h"
#include "entity.h"
#include "raylib.h"
#include <cstdio>
#include <vector>

#define INPUT_DEBOUNCE 10;

void Engine::renderFrame() {
  handleInputs();
  handleCollisions();
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

void Engine::addEntity(Entity *entity) {
  entities.push_back(entity);
}

void Engine::addEntities(std::vector<Entity *> *entities) {
  for (Entity* each: *entities) {
    addEntity(each);
  }
}

void Engine::setInputHandler(Entity *entity, InputHandler handler) {
  InputHandlerState *state = new InputHandlerState;
  state->entity = entity;
  state->handler = handler;
  state->debounceFrames = 0;
  inputHandlers.push_back(state);
}

Engine::~Engine() {
  while (!inputHandlers.empty()) {
    InputHandlerState *each = inputHandlers.back();
    delete each;
    inputHandlers.pop_back();
  }
}

void Engine::handleCollisions() {
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

void Engine::handleInputs() {
  for (InputHandlerState *each : inputHandlers) {
    if (each->debounceFrames > 0) {
      each->debounceFrames--;
      continue;
    }
    if (each->handler(each->entity)) {
      each->debounceFrames = INPUT_DEBOUNCE;
    }
  }
}
