#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "GrassUtils.hpp"

class GrassAssets {
public:
    std::vector<Blade> blades;
    int shadeAmount;
    Shader burnShader;
    int burningIntensityLoc;
    int paletteLoc;
    GrassAssets(const std::string& path, int shadeAmount);
    ~GrassAssets() {
        // Unload textures when done.
        printf("Unloading Textures\n");
        for (auto& blade : blades) {
            UnloadTexture(blade.texture);
        }
        UnloadShader(burnShader);
    };
    void renderBlade(int id, Vector2 location, int rotation, float scale, Color palette);
};
