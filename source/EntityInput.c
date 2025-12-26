#include "raylib.h"
#include "stdlib.h"

#include "EntityInput.h"

#include "Entity.h"
#include "Graph.h"

#include "Program.h"
#include "UIState.h"

#include "stdio.h"


void InitUI() {
    gSelectNode = -1;
    gHoldNode = -1;
    gTextDisplaySelected = -1;
}


void SelectNode(Vector2 mousePos) {
    if (gHoldNode != -1) return;
    if (gTextDisplaySelected != -1) return;

    if (gSelectNode != -1) {
        gSelectNode = -1;
    }

    for (int i = 0; i < gNodes.length; ++i) {
        const Node node = gNodes.data[i];
        if (Vector2Distance(mousePos, node.pos) > NODE_RADIUS) continue;
        gSelectNode = i;
        gHoldNode = gSelectNode;    
        return;
    }

    Node n = {
        .pos = mousePos,
        .numNextTasks = 0,
        .numPrevTasks = 0,
        .processor = 0,
        .endTime = 0,
        .startTime = 0,
        .id = gNodes.length
    };

    AddNode(n);

    for (int i = 0; i < gProcessors.length; ++i) {
        gProcessors.data[i].computeCosts = (int *) realloc( gProcessors.data[i].computeCosts, sizeof(int) * gNodes.length);
        gProcessors.data[i].computeCosts[gNodes.length - 1] = 0;
    }

    gTextDisplaySelected = -1;
}


void MoveNode(Vector2 mousePos) {
    if (gHoldNode != -1) {
        gNodes.data[gHoldNode].pos = mousePos;
    }
}


void UnholdNode() { 
    gHoldNode = -1;
}


Edge gEdge = {
    .fromNodeId = -1,
    .toNodeId = -1
};


void SelectEdge(Vector2 mousePos) {
    
    for (int i = 0; i < gNodes.length; ++i) {
        const Node node = gNodes.data[i];
        if (Vector2Distance(mousePos, node.pos) > NODE_RADIUS) continue;
        
        if (gEdge.fromNodeId == -1) {
            gEdge.fromNodeId = i;
            return;
        }
    
        if (i == gEdge.fromNodeId) return;

        gEdge.toNodeId = i;
                
        gNodes.data[gEdge.fromNodeId].numNextTasks += 1;
        gNodes.data[gEdge.toNodeId].numPrevTasks += 1;
        printf("toNodeid %i prev %i\n", gEdge.toNodeId, gNodes.data[gEdge.toNodeId].numPrevTasks);

        gEdge.commCost = 0;
        AddEdge(gEdge);

        TextDisplay weight;
        InitTextDisplay(&weight, TD_EDGE_WEIGHT, gEdges.length - 1);
        AddTextDisplay(weight);
        
        gEdge.fromNodeId = -1;
        gEdge.toNodeId = -1;

        return;
    }

    gEdge.fromNodeId = -1;
    gEdge.toNodeId = -1;
}



void SelectTextDisplay(Vector2 mousePos) {
    for (int i = 0; i < gTextDisplays.length; ++i) {
        const TextDisplay weight = gTextDisplays.data[i];

        const int expand = 5;
        const float topLeftX = weight.pos.x - expand;
        const float topLeftY = weight.pos.y - expand;
        const float bottomRightX = weight.pos.x + weight.textWidth + expand;
        const float bottomRightY = weight.pos.y + WEIGHT_FONT_SIZE + expand;

        if (!(mousePos.x >= topLeftX &&
            mousePos.y >= topLeftY && 
            mousePos.x <= bottomRightX && 
            mousePos.y <= bottomRightY)) continue;
        
        if (gSelectNode == -1 && weight.weightType == TD_PROCESSOR_COST) return;

        gTextDisplaySelected = i;
        return;
    }
    gTextDisplaySelected = -1;
}


void WriteNumberTextDisplay(int number) {
    if (gTextDisplaySelected == -1) return;

    const int weightType = gTextDisplays.data[gTextDisplaySelected].weightType;
    const int typeId = gTextDisplays.data[gTextDisplaySelected].typeId;

    switch (weightType) {
        case TD_EDGE_WEIGHT: {
            if (number == -1) {
                gEdges.data[typeId].commCost = (int)gEdges.data[typeId].commCost / 10;
            } else {
                gEdges.data[typeId].commCost = gEdges.data[typeId].commCost * 10 + number;
            }
        } break;
        case TD_PROCESSOR_COST: {
            if (gSelectNode == -1) break;
            if (number == -1) {
                gProcessors.data[typeId].computeCosts[gSelectNode] /= 10;
            } else {
                gProcessors.data[typeId].computeCosts[gSelectNode] *= 10;
                gProcessors.data[typeId].computeCosts[gSelectNode] += number;
            }
        } break;
        default: break;
    }

    InitTextDisplay(&gTextDisplays.data[gTextDisplaySelected], weightType, typeId);
}


void DrawInput() {
    if (gEdge.fromNodeId != -1) {
        const Node node = gNodes.data[gEdge.fromNodeId];
        DrawCircleLines(node.pos.x, node.pos.y, NODE_RADIUS + 2, GREEN);
    }
    
    if (gEdge.toNodeId != -1) {
        const Node node = gNodes.data[gEdge.toNodeId];
        DrawCircleLines(node.pos.x, node.pos.y, NODE_RADIUS + 2, GREEN);
    }

    if (gSelectNode != -1 && gTextDisplaySelected == -1) {
        const Node node = gNodes.data[gSelectNode];
        DrawCircleLines(node.pos.x, node.pos.y, NODE_RADIUS + 2, BLUE);
        
        char nodeId[10];
        snprintf(nodeId, 10, "%i", node.id);
        DrawText(nodeId, 0, WINDOW_H - WEIGHT_TEXT_SELECTED_FONT_SIZE, WEIGHT_TEXT_SELECTED_FONT_SIZE, GREEN);
    }

    if (gTextDisplaySelected != -1) {
        const TextDisplay weight = gTextDisplays.data[gTextDisplaySelected];
        DrawText(weight.text, 0, WINDOW_H - WEIGHT_TEXT_SELECTED_FONT_SIZE, WEIGHT_TEXT_SELECTED_FONT_SIZE, GREEN);
        const Rectangle rect = {
            weight.pos.x - 1, weight.pos.y - 1, weight.textWidth + 2, WEIGHT_FONT_SIZE + 2
        };
        DrawRectangleLinesEx(rect, 1.0, GREEN);
    }


}


void CreateNewProcessor() {
    Processor processor = {
        .computeCosts = (int *) malloc(sizeof(int) * gNodes.length),
    };

    for (int i = 0; i < gNodes.length; ++i) {
        processor.computeCosts[i] = 0;
    }

    processor.id = gProcessors.length;
    
    AddProcessor(processor);

    TextDisplay weight;
    InitTextDisplay(&weight, TD_PROCESSOR_COST, gProcessors.length - 1);
    AddTextDisplay(weight);
}