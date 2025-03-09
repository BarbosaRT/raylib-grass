#pragma once
#include <raylib.h>
#include <vector>
// ---------------------- Global Constants ---------------------- //
// Offsets for checking nearby tiles when burning (like BURN_CHECK_OFFSETS)
const std::vector<Vector2> BURN_CHECK_OFFSETS = {
    {-1, -1}, { 0, -1}, { 1, -1},
    {-1,  0},           { 1,  0},
    {-1,  1}, { 0,  1}, { 1,  1}
};

// ------------------------- Structs ----------------------------- //
struct GroundShadow {
    int radius;
    Color color;
    int strength;
    float shift;
};

struct GrassConfiguration {
    int tileSize;
    int shadeAmount;
    int stiffness;
    int maxUnique;
    int verticalPlaceRange[2];
    GroundShadow groundShadow;
    int padding;
};

struct Blade {
    Texture2D texture;
    Color averageColor;
};

struct BladeData {
    Vector2 offset;
    int bladeId;
    float rotation;
    Color palette;
};

// ----------------------- Functions --------------------------- //

float normalize(float current, float rate, float target);

// ------------------------- Classes -------------------------- //
class Cache {
};
