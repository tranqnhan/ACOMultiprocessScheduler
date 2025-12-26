#include "raylib.h"
#include "raymath.h"

#include "DrawHelper.h"

void DrawArrow(Vector2 start, Vector2 end, Color color)
{
    const float headSize = 10.0;
    const float thickness = 2.0;

    // Draw main line
    DrawLineEx(start, end, thickness, color);

    // Direction vector
    Vector2 dir = Vector2Normalize(Vector2Subtract(end, start));

    // Perpendicular vectors for arrow head
    Vector2 right = { -dir.y, dir.x };
    Vector2 left  = {  dir.y, -dir.x };

    // Arrow head endpoints
    Vector2 p1 = {
        end.x - dir.x * headSize + right.x * headSize * 0.5f,
        end.y - dir.y * headSize + right.y * headSize * 0.5f
    };

    Vector2 p2 = {
        end.x - dir.x * headSize + left.x * headSize * 0.5f,
        end.y - dir.y * headSize + left.y * headSize * 0.5f
    };

    DrawLineEx(end, p1, thickness, color);
    DrawLineEx(end, p2, thickness, color);
}
