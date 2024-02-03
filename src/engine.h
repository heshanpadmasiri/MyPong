#ifndef ENGINE_H_
#define ENGINE_H_

// 1. Create an Engine object
// 2. Add entities to engine
// 3. Add user input handlers (entity, fnPtr)
#include "entity.h"
#include <utility>
#include <vector>

typedef void (*InputHandlerFn)(Entity *);

class Engine {
public:
  void addEntity(Entity *entity);
  void setInputHandler(Entity *entity, InputHandlerFn handler);
  void renderFrame();
  ~Engine();

private:
  std::vector<Entity *> entities;
  std::vector<std::pair<Entity *, InputHandlerFn>*> inputHandlers;
  void handleCollisions();
};

#endif // ENGINE_H_
