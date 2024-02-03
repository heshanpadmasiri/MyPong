#include "engine.h"
#include "entity.h"
#include "raylib.h"
#include <utility>
#include <vector>

void Engine::addEntity(Entity *entity) { this->entities.push_back(entity); }

void Engine::setInputHandler(Entity *entity, InputHandlerFn handler) {
  std::pair<Entity *, InputHandlerFn> *pair =
      new std::pair<Entity *, InputHandlerFn>(entity, handler);
  this->inputHandlers.push_back(pair);
}

void Engine::renderFrame() {
  for (auto handlerPair : this->inputHandlers) {
    handlerPair->second(handlerPair->first);
  }
  this->handleCollisions();
  for (Entity *entity : this->entities) {
    entity->update(GetFrameTime());
  }
  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (Entity *entity : entities) {
    entity->draw();
  }
  EndDrawing();
}

void Engine::handleCollisions() {
  std::vector<Entity *> entities = this->entities;
  for (size_t i = 0; i < entities.size(); i++) {
    for (size_t j = i + 1; j < entities.size(); j++) {
      Entity *e1 = entities.at(i);
      Entity *e2 = entities.at(j);
      if (e1->couldSkipCollisionCheck() && e2->couldSkipCollisionCheck()) {
        continue;
      }
      if (isColliding(e1, e2)) {
        resolveCollision(e2, e1);
      }
    }
  }
}

Engine::~Engine() {
  while (!this->inputHandlers.empty()) {
    std::pair<Entity *, InputHandlerFn> *handler = this->inputHandlers.back();
    this->inputHandlers.pop_back();
    delete handler;
  }
  while (!this->entities.empty()) {
    Entity *e = entities.back();
    entities.pop_back();
    delete e;
  }
}
