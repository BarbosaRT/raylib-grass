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
    Vector2 shift;
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
#include <memory>  // for std::unique_ptr

class Cache {
public:
    // Public static method to access the singleton instance
    static Cache& get() {
        static Cache instance;  // This will be the only instance of Cache
        return instance;
    }

    int grassId = 0;

private:
    // Private constructor to prevent instantiation
    Cache() {
        // Initialize the cache here if needed
    }

    // Deleted copy constructor and assignment operator to ensure only one instance
    Cache(const Cache&) = delete;
    Cache& operator=(const Cache&) = delete;
};
