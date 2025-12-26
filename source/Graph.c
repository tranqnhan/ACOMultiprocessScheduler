#include "UIState.h"
#include "stdio.h"
#include "stdlib.h"

#include "raylib.h"
#include "raymath.h"

#include "DrawHelper.h"
#include "Entity.h"
#include "Graph.h"


void DrawEdge(Edge *e) {

    const Node *node1 = &gNodes.data[e->fromNodeId];
    const Node *node2 = &gNodes.data[e->toNodeId];

    // Direction vector
    Vector2 dir = Vector2Normalize(Vector2Subtract(node2->pos, node1->pos));

    const float shiftX = dir.x * NODE_RADIUS;
    const float shiftY = dir.y * NODE_RADIUS;

    Vector2 start = {
        .x = node1->pos.x + shiftX,
        .y = node1->pos.y + shiftY
    };

    Vector2 end = {
        .x = node2->pos.x - shiftX,
        .y = node2->pos.y - shiftY
    };

    DrawArrow(start, end, WHITE);
}

void UpdateEdge(Edge *e, float deltaTime) {
    return;
}

void DrawNode(Node *n) {
    DrawCircle(n->pos.x, n->pos.y, NODE_RADIUS, WHITE);
    
    char pname[10];
    snprintf(pname, 9, "%i", n->processor);
    const float textWidth = MeasureText(pname, WEIGHT_FONT_SIZE);

    const int x = n->pos.x - (textWidth / 2);
    const int y = n->pos.y - ((float)WEIGHT_FONT_SIZE / 2);

    DrawText(pname, x, y, WEIGHT_FONT_SIZE, BLACK);

    char taskEndTime[10];
    snprintf(taskEndTime, 10, "%i", n->endTime);
    const float eTextWidth = MeasureText(taskEndTime, WEIGHT_FONT_SIZE);
    DrawText(taskEndTime,
        n->pos.x - (eTextWidth / 2) + WEIGHT_NODE_OFFSET, 
        n->pos.y - ((float)WEIGHT_FONT_SIZE / 2) + WEIGHT_NODE_OFFSET, 
        WEIGHT_FONT_SIZE,
        RED);

    char taskStartTime[10];
    snprintf(taskStartTime, 10, "%i", n->startTime);
    const float sTextWidth = MeasureText(taskStartTime, WEIGHT_FONT_SIZE);
    DrawText(taskStartTime,
        n->pos.x - (sTextWidth / 2) - WEIGHT_NODE_OFFSET, 
        n->pos.y - ((float)WEIGHT_FONT_SIZE / 2) + WEIGHT_NODE_OFFSET, 
        WEIGHT_FONT_SIZE,
        BLUE);
}

void UpdateNode(Node *n, float deltaTime) {
    return;
}


void DrawProcessor(Processor *p) {
    char pname[10];
    snprintf(pname, 10, "P%i", p->id);

    DrawText(pname, WEIGHT_FONT_SIZE, p->id * WEIGHT_FONT_SIZE + WEIGHT_FONT_SIZE, WEIGHT_FONT_SIZE, WHITE);
}


void UpdateProcessor(Processor *p, float deltaTime) {
    return;
}


void InitTextDisplay(TextDisplay *w, int weightType, int typeId) {
    w->weightType = weightType;
    w->typeId = typeId;

    switch (w->weightType) {
        case TD_EDGE_WEIGHT: {
            const Edge e = gEdges.data[w->typeId];
            snprintf(w->text, MAX_WEIGHT_TEXT_LEN, "%i", e.commCost);
            w->textWidth = MeasureText(w->text, WEIGHT_FONT_SIZE);

            const Node *n1 = &gNodes.data[e.fromNodeId];
            const Node *n2 = &gNodes.data[e.toNodeId];

    
            const Vector2 fromPos = n1->pos;
            const Vector2 toPos = n2->pos;

            const float x = (fromPos.x + toPos.x) / 2;
            const float y = (fromPos.y + toPos.y) / 2;
            
            w->pos.x = (x - (w->textWidth / 2));
            w->pos.y = (y - ((float)WEIGHT_FONT_SIZE / 2));
        } break;
        case TD_PROCESSOR_COST: {
            const Processor p = gProcessors.data[w->typeId];

            if (gSelectNode == -1) {
                snprintf(w->text, MAX_WEIGHT_TEXT_LEN, "%s", "_");
            } else {
                snprintf(w->text, MAX_WEIGHT_TEXT_LEN, "%i", p.computeCosts[gSelectNode]);
            }

            w->textWidth = MeasureText(w->text, WEIGHT_FONT_SIZE);

            w->pos.x = WEIGHT_FONT_SIZE * 3;
            w->pos.y = p.id * WEIGHT_FONT_SIZE + WEIGHT_FONT_SIZE;
        } break;
        default: return;
    }
}


void UpdateTextDisplay(TextDisplay *w, float deltaTime) {
    switch (w->weightType) {
        case TD_EDGE_WEIGHT: {
            const Edge e = gEdges.data[w->typeId];
            snprintf(w->text, MAX_WEIGHT_TEXT_LEN, "%i", e.commCost);
            w->textWidth = MeasureText(w->text, WEIGHT_FONT_SIZE);

            const Node *n1 = &gNodes.data[e.fromNodeId];
            const Node *n2 = &gNodes.data[e.toNodeId];

    
            const Vector2 fromPos = n1->pos;
            const Vector2 toPos = n2->pos;

            const float x = (fromPos.x + toPos.x) / 2;
            const float y = (fromPos.y + toPos.y) / 2;
            
            w->pos.x = (x - (w->textWidth / 2));
            w->pos.y = (y - ((float)WEIGHT_FONT_SIZE / 2));
        } break;
        case TD_PROCESSOR_COST: {
            const Processor p = gProcessors.data[w->typeId];

            if (gSelectNode == -1) {
                snprintf(w->text, MAX_WEIGHT_TEXT_LEN, "%s", "_");
            } else {
                snprintf(w->text, MAX_WEIGHT_TEXT_LEN, "%i", p.computeCosts[gSelectNode]);
            }

            w->textWidth = MeasureText(w->text, WEIGHT_FONT_SIZE);

            w->pos.x = WEIGHT_FONT_SIZE * 3;
            w->pos.y = p.id * WEIGHT_FONT_SIZE + WEIGHT_FONT_SIZE;
        } break;
        default: return;
    }
}


void DrawTextDisplay(TextDisplay *w) {
    DrawRectangle(w->pos.x, w->pos.y, w->textWidth, WEIGHT_FONT_SIZE, BLACK);
    
    //const float topLeftX = w->pos.x;
    //const float topLeftY = w->pos.y;
    //const float bottomRightX = w->pos.x + w->textWidth;
    //const float bottomRightY = w->pos.y + WEIGHT_FONT_SIZE;
    //
    //DrawCircle(topLeftX, topLeftY, 1, GREEN);
    //DrawCircle(bottomRightX, bottomRightY, 1, GREEN);

    DrawText(w->text, w->pos.x, w->pos.y, WEIGHT_FONT_SIZE, WHITE);
}


GraphNode *AllocGraph() {
    GraphNode *graph = (GraphNode *) malloc(sizeof(GraphNode) * gNodes.length);

    for (int i = 0; i < gNodes.length; ++i) {
        graph[i].nextTasks = (int *) malloc(sizeof(int) * gNodes.data[i].numNextTasks);
        graph[i].commCosts = (int *) malloc(sizeof(int) * gNodes.data[i].numNextTasks);

        graph[i].numNextTasks = 0;    
        graph[i].uRank = -1;
        graph[i].estimatedStartTime = -1;
        graph[i].averageComputateCost = -1;
    } 

    for (int i = 0; i < gEdges.length; ++i) {
        const int fromNodeId = gEdges.data[i].fromNodeId;
        const int toNodeId = gEdges.data[i].toNodeId;

        graph[fromNodeId].nextTasks[graph[fromNodeId].numNextTasks] = toNodeId;
        graph[fromNodeId].commCosts[graph[fromNodeId].numNextTasks] = gEdges.data[i].commCost;
        
        graph[fromNodeId].numNextTasks += 1;
    }

    return graph;
}


void FreeGraph(GraphNode *graph) {
    for (int i = 0; i < gNodes.length; ++i) {
        free(graph[i].nextTasks);
        free(graph[i].commCosts);
    }

    free(graph);
}

