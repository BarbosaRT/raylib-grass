#include "GrassAssets.hpp"
#include <string>
#include <filesystem>
#include <iostream>

GrassAssets::GrassAssets(const std::string& path, int shadeAmount) : shadeAmount(shadeAmount) {
    // Iterate over all files in the given folder (e.g., "assets/grass")
    burnShader = LoadShader(0, "assets/shaders/burning.fs");
    burningIntensityLoc = GetShaderLocation(burnShader, "burningIntensity");
    //paletteLoc = GetShaderLocation(burnShader, "palette");

    std::cout << "\n";
    std::cout << "\n";
    std::cout << "burningIntensityLoc: " << burningIntensityLoc << std::endl;
    std::cout << "paletteLoc: " << paletteLoc << std::endl;
    std::cout << "\n";
    std::cout << "\n";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string filePath = entry.path().string();
        Texture2D tex = LoadTexture(filePath.c_str());
        // Load the image to compute its average color
        Image img = LoadImage(filePath.c_str());
        int sumR = 0, sumG = 0, sumB = 0, count = 0;
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                Color pixel = GetImageColor(img, x, y);
                // Ignore black pixels (assumed to be transparent)
                if (!(pixel.r == 0 && pixel.g == 0 && pixel.b == 0)) {
                    sumR += pixel.r;
                    sumG += pixel.g;
                    sumB += pixel.b;
                    count++;
                }
            }
        }
        Color avgColor = { 0, 0, 0, 255 };
        if (count > 0) {
            avgColor.r = sumR / count;
            avgColor.g = sumG / count;
            avgColor.b = sumB / count;
        }
        UnloadImage(img);
        Blade blade;
        blade.texture = tex;
        blade.averageColor = avgColor;
        blades.push_back(blade);
    }
}

void GrassAssets::renderBlade(int id, Vector2 location, int rotation, float scale, Color palette)
{
    if (id < 0 || id >= blades.size()) return;
    Texture2D tex = blades[id].texture;
    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
    float width = tex.width * scale;
    float height = tex.height * scale;
    Rectangle dest = { location.x - width / 2, location.y - height / 2, width, height };
    Vector2 origin = { width / 2, height / 2 };

    // Compute a tint based on rotation and the configured shade amount.
    float alpha = (float)shadeAmount * (fabs(rotation) / 90.0f);
    alpha = 255.0f-alpha; //Invert.
    alpha = std::max(40.0f,alpha); //make sure it is never below 40
    Color tint = { 255, 255, 255, alpha };
    //TODO: OPTIMIZE IT
    // If the blade is burning (scale less than 1), modify tint using the blades palette.
    /*if (scale < 1.0f && scale > 0.0f) {
        tint.r = fmin(255, palette.r * 1.8f * (6.0f / scale));
        tint.g = fmin(255, palette.g * (1.0f / scale));
        tint.b = fmin(255, palette.b * (1.0f / scale));

        Vector3 burning = {tint.r / 255, tint.g / 255, tint.b/ 255};     
        BeginShaderMode(burnShader);
        SetShaderValue(burnShader, burningIntensityLoc, &scale, SHADER_UNIFORM_FLOAT);
        SetShaderValue(burnShader, paletteLoc, &burning, SHADER_UNIFORM_VEC3);

        DrawTexturePro(tex, source, dest, origin, rotation, tint);
        EndShaderMode();
    }
    else {
        DrawTexturePro(tex, source, dest, origin, rotation, tint);
    }*/

    if (scale < 1.0f && scale > 0.0f) {
    // Calculate normalized color components with adjusted multipliers
    
        tint.r = fmin(255, palette.r * 1.8f * (6.0f / scale));
        tint.g = fmin(255, palette.g * (1.0f / scale));
        tint.b = fmin(255, palette.b * (1.0f / scale));

        Vector3 burning = {tint.r / 200, tint.g / 200, tint.b/ 200};   
    
        // Use burning intensity = 1 - scale
        float burningIntensity = scale;
        
        BeginShaderMode(burnShader);
        SetShaderValue(burnShader, burningIntensityLoc, &burningIntensity, SHADER_UNIFORM_FLOAT);
        SetShaderValue(burnShader, paletteLoc, &burning, SHADER_UNIFORM_VEC3);

        DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
        EndShaderMode();
}
else {
    DrawTexturePro(tex, source, dest, origin, rotation, tint);
}
}

