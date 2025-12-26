#include "Entity.h"


#define X(name) ENTITY_DECL(name)
ENTITY_TYPES
#undef X

void UpdateEntities(float deltaTime) {
#define X(name) Update##name##s(deltaTime);
ENTITY_TYPES
#undef X
}

void DrawEntities() {
#define X(name) Draw##name##s();
ENTITY_TYPES
#undef X
}

