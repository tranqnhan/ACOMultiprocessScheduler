
#pragma once

#include "assert.h"
#include "raymath.h"

#include "Graph.h"

#define MAX_ENTITY 512

#define ENTITY_TYPES \
    X(Node) \
    X(Processor) \
    X(Edge) \
    X(TextDisplay) \

#define ENTITY_DEF(name) \
typedef struct { \
    int length; \
    name data[MAX_ENTITY]; \
} name##Array; \
extern name##Array g##name##s;\
void Add##name(name n);\
void Draw##name##s();\
void Update##name##s(float deltaTime);\

#define X(name) ENTITY_DEF(name) 
ENTITY_TYPES 
#undef X

#define ENTITY_DECL(name)\
name##Array g##name##s = {0};\
\
void Add##name(name n) {\
    assert(g##name##s.length < MAX_ENTITY); \
    g##name##s.data[g##name##s.length] = n;\
    g##name##s.length += 1;\
}\
\
\
void Update##name##s(float deltaTime) {\
    for (int i = 0; i < g##name##s.length; ++i) { \
        Update##name(&g##name##s.data[i], deltaTime); \
    } \
}\
void Draw##name##s() {\
    for (int i = 0; i < g##name##s.length; ++i) { \
        Draw##name(&g##name##s.data[i]); \
    } \
}\


void UpdateEntities(float deltaTime);
void DrawEntities();
