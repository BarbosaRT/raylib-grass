#include "GrassAssets.hpp"
#include <string>
#include <filesystem>

GrassAssets::GrassAssets(const std::string& path, int shadeAmount) : shadeAmount(shadeAmount) {
    // Iterate over all files in the given folder (e.g., "assets/grass")
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
            avgColor.r = (unsigned char)(sumR / count);
            avgColor.g = (unsigned char)(sumG / count);
            avgColor.b = (unsigned char)(sumB / count);
        }
        UnloadImage(img);
        blades.push_back({ tex, avgColor });
    }
}

void GrassAssets::renderBlade(int id, Vector2 location, int rotation, int scale, Color palette)
{
    if (id < 0 || id >= (int)blades.size()) return;
    Texture2D tex = blades[id].texture;
    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
    float width = tex.width * scale;
    float height = tex.height * scale;
    Rectangle dest = { location.x - width / 2, location.y - height / 2, width, height };
    Vector2 origin = { width / 2, height / 2 };

    // Compute a tint based on rotation and the configured shade amount.
    float alpha = shadeAmount * (fabs(rotation) / 90.0f);
    Color tint = { 255, 255, 255, (unsigned char)alpha };

    // If the blade is burning (scale less than 1), modify tint using the blades palette.
    if (scale < 1.0f && scale > 0.0f) {
        tint.r = (unsigned char)fmin(255, palette.r * 1.8f * (6.0f / scale));
        tint.g = (unsigned char)fmin(255, palette.g * (1.0f / scale));
        tint.b = (unsigned char)fmin(255, palette.b * (1.0f / scale));
    }
    DrawTexturePro(tex, source, dest, origin, rotation, tint);
}

