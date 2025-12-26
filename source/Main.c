#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "raylib.h"

#include "Graph.h"
#include "Entity.h"
#include "EntityInput.h"
#include "Program.h"
#include "ACORNK.h"

void LoadFromFile() {
    FILE *fp;
    fp = fopen("../config/input.txt", "r");
    if (fp == NULL) {
        printf("No graph diagram to open.\n");
        return;
    }
    
    char buffer[MAX_LINE];

    while (fgets(buffer, MAX_LINE, fp) != NULL) {
        if (strcmp(buffer, "\n") == 0) break;
        char *token;
        const char delimiter[] = " ";
        
        Node node;

        token = strtok(buffer, delimiter);
        node.pos.x = atof(token);
        token = strtok(NULL, delimiter);
        node.pos.y = atof(token);
        token = strtok(NULL, delimiter);
        node.processor = atoi(token);
    
        node.numNextTasks = 0;
        node.numPrevTasks = 0;
        node.startTime = 0;
        node.endTime = 0;
        node.id = gNodes.length;

        AddNode(node);
    }


    while (fgets(buffer, MAX_LINE, fp) != NULL) {
        if (strcmp(buffer, "\n") == 0) break;
        char *token;
        const char delimiter[] = " ";
        
        Edge edge;

        token = strtok(buffer, delimiter);
        edge.fromNodeId = atoi(token);
        token = strtok(NULL, delimiter);
        edge.toNodeId = atoi(token);
        token = strtok(NULL, delimiter);
        edge.commCost = atoi(token);

        gNodes.data[edge.fromNodeId].numNextTasks++;
        gNodes.data[edge.toNodeId].numPrevTasks++;

        AddEdge(edge);
    
        TextDisplay weight;
        InitTextDisplay(&weight, TD_EDGE_WEIGHT, gEdges.length - 1);
        AddTextDisplay(weight);

        
    }
    
    while (fgets(buffer, MAX_LINE, fp) != NULL) {
        if (strcmp(buffer, "\n") == 0) break;
        char *token;
        const char delimiter[] = " ";
        
        CreateNewProcessor();
        
        token = strtok(buffer, delimiter);
        for (int i = 0; i < gNodes.length; ++i) {
            gProcessors.data[gProcessors.length - 1].computeCosts[i] = atoi(token);
            token = strtok(NULL, delimiter);
        }

    
        TextDisplay weight;
        InitTextDisplay(&weight, TD_PROCESSOR_COST, gProcessors.length - 1);
        AddTextDisplay(weight);
    }

    fclose(fp);
}

void SaveToFile() {
    FILE *fp;
    fp = fopen("../config/input.txt", "w+");
    if (fp == NULL) {
        printf("Cannot save graph to file.\n");
        return;
    }
    
    for (int i = 0; i < gNodes.length; ++i) {
        char out[MAX_LINE];
        snprintf(out, MAX_LINE, "%f %f %i\n", 
            gNodes.data[i].pos.x, 
            gNodes.data[i].pos.y,
            gNodes.data[i].processor
        );
        fprintf(fp, "%s", out);
    }

    fprintf(fp, "%s", "\n");

    for (int i = 0; i < gEdges.length; ++i) {
        char out[MAX_LINE];
        snprintf(out, MAX_LINE, "%i %i %i\n", 
            gEdges.data[i].fromNodeId, 
            gEdges.data[i].toNodeId,
            gEdges.data[i].commCost            
        );
        fprintf(fp, "%s", out);
    }
    
    fprintf(fp, "%s", "\n");

    for (int i = 0; i < gProcessors.length; ++i) {
        for (int j = 0; j < gNodes.length; ++j) {
            char out[MAX_LINE];
            snprintf(out, MAX_LINE, "%i",
                gProcessors.data[i].computeCosts[j]
            );
            fprintf(fp, "%s ", out);
        }
        fprintf(fp, "%s", "\n");
    }
    fclose(fp);
    printf("Saved!\n");
}


void Init() {
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    LoadFromFile();
    SetTargetFPS(60);
    srand(0);

    InitAlgorithm();
    InitUI();
}


// Main loop update
void Update(float deltaTime) {
    UpdateEntities(deltaTime);
    UpdateACORNK(deltaTime);
}


// Main loop input
void Input() {
    Vector2 mousePos = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        SelectEdge(mousePos);
    }

    // Node Inputs
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        SelectTextDisplay(mousePos);
        SelectNode(mousePos);
    }

    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
        UnholdNode();
    }

    MoveNode(mousePos);

    if (IsKeyPressed(KEY_P)) {
        CreateNewProcessor();
    }

    if (IsKeyPressed(KEY_S)) {
        SaveToFile();
    }

    if (IsKeyPressed(KEY_ZERO))  WriteNumberTextDisplay(0);
    if (IsKeyPressed(KEY_ONE))   WriteNumberTextDisplay(1);
    if (IsKeyPressed(KEY_TWO))   WriteNumberTextDisplay(2);
    if (IsKeyPressed(KEY_THREE)) WriteNumberTextDisplay(3);
    if (IsKeyPressed(KEY_FOUR))  WriteNumberTextDisplay(4);
    if (IsKeyPressed(KEY_FIVE))  WriteNumberTextDisplay(5);
    if (IsKeyPressed(KEY_SIX))   WriteNumberTextDisplay(6);
    if (IsKeyPressed(KEY_SEVEN)) WriteNumberTextDisplay(7);
    if (IsKeyPressed(KEY_EIGHT)) WriteNumberTextDisplay(8);
    if (IsKeyPressed(KEY_NINE))  WriteNumberTextDisplay(9);
    if (IsKeyPressed(KEY_BACKSPACE))  WriteNumberTextDisplay(-1);


    if (IsKeyPressed(KEY_SPACE)) {
        BeginAlgorithm();
    }

}

// Main loop draw
void Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawFPS(0, 0);
    
    DrawEntities();
    DrawInput();

    // Draw algorithm best solution cost
    const char *bestSolutionTxt = "COST: ";
    char algo[strlen(bestSolutionTxt) + 10];
    snprintf(algo, strlen(bestSolutionTxt) + 10, "%s%i", bestSolutionTxt, gACORNK.bestSolution);
    const float textWidth = MeasureText(algo, WEIGHT_FONT_SIZE);
    DrawText(algo, WINDOW_W - textWidth - WEIGHT_FONT_SIZE, WEIGHT_FONT_SIZE, WEIGHT_FONT_SIZE, WHITE);

    EndDrawing();
}


// main
int main(void) {
    Init();

    while (!WindowShouldClose()) {
        Input();
        Update(GetFrameTime());
        Draw();
    }

    CloseWindow();
    return 0;
}
