#pragma once

#include "raymath.h"

#define NODE_RADIUS 30

#define TD_EDGE_WEIGHT 0
#define TD_PROCESSOR_COST 1

#define MAX_WEIGHT_TEXT_LEN 64
#define WEIGHT_FONT_SIZE 24
#define WEIGHT_NODE_OFFSET 18

#define START_NODE 0
#define END_NODE 0

typedef struct {
    int fromNodeId;
    int toNodeId;
    int commCost;
} Edge;

void DrawEdge(Edge *e);
void UpdateEdge(Edge *e, float deltaTime);
void DrawEdge(Edge *e);

typedef struct {
    Vector2 pos;
    int numNextTasks;
    int numPrevTasks;
    int processor;
    
    int id;
    int endTime;
    int startTime;
} Node;

void DrawNode(Node *n);
void UpdateNode(Node *n, float deltaTime);


typedef struct {
    char text[MAX_WEIGHT_TEXT_LEN];
    int typeId;
    char weightType;
    Vector2 pos;
    float textWidth;
} TextDisplay;

void DrawTextDisplay(TextDisplay *w);
void UpdateTextDisplay(TextDisplay *n, float deltaTime);
void InitTextDisplay(TextDisplay *w, int weightType, int typeId);

typedef struct {
    int *computeCosts;
    int id;
} Processor;

void DrawProcessor(Processor *p);
void UpdateProcessor(Processor *n, float deltaTime);

typedef struct {
    int *nextTasks;
    int *commCosts;
    
    int numNextTasks;
    
    float estimatedStartTime;
    float uRank;
    float averageComputateCost;
} GraphNode;

GraphNode *AllocGraph();
void FreeGraph(GraphNode *graph);