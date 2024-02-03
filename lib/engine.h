#ifndef ENGINE_H_
#define ENGINE_H_

#include "entity.h"
#include <utility>
#include <vector>

typedef void (*InputHandlerFn)(Entity *);
typedef std::pair<Entity *, InputHandlerFn> InputHandler;

struct GameState {
  std::vector<Entity *> entities;
  std::vector<InputHandler *> inputHandlers;
};

typedef void (*RenderFn)(GameState*);

void renderFrame(GameState *state);

#endif // ENGINE_H_
