#pragma once

#include "raymath.h"

#define WEIGHT_TEXT_SELECTED_FONT_SIZE 30

void SelectNode(Vector2 mousePos);
void MoveNode(Vector2 mousePos);
void UnholdNode();
void SelectEdge(Vector2 mousePos);
void SelectTextDisplay(Vector2 mousePos);
void WriteNumberTextDisplay(int number);

void InitUI();
void DrawInput();
void CreateNewProcessor();