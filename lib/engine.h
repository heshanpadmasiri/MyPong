#ifndef ENGINE_H_
#define ENGINE_H_

#include "entity.h"
#include <cstdint>
#include <vector>

typedef bool (*InputHandler)(Entity *);

struct InputHandlerState {
  uint64_t debounceFrames;
  Entity *entity;
  InputHandler handler;
};

class Engine {
public:
  void renderFrame();
  void addEntity(Entity *);
  void addEntities(std::vector<Entity *> *entities);
  void setInputHandler(Entity *entity, InputHandler handler);
  ~Engine();

private:
  void handleCollisions();
  void handleInputs();
  std::vector<Entity *> entities;
  std::vector<InputHandlerState*> inputHandlers;
};

#endif // ENGINE_H_
