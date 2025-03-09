#include "GrassTile.hpp"
#include <random>
#include <raymath.h>
GrassTile::GrassTile(int tileSize, Vector2 location, int amount, const std::vector<int>& grassOptions, GrassConfiguration gc, GrassAssets* grassAssets) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disFloat(0.0f, 1.0f);
    std::uniform_real_distribution<float> disRot(-15.0f, 15.0f);
    grassConfiguration = gc;
    float range = grassConfiguration.verticalPlaceRange[1] - grassConfiguration.verticalPlaceRange[0];
	for (int i = 0; i < amount; i++) {
        int newBlade = grassOptions[rand() % grassOptions.size()];
        float yPos = grassConfiguration.verticalPlaceRange[0];
        if (range > 0) {
            yPos = rand() * range + grassConfiguration.verticalPlaceRange[0];
        }
        BladeData blade;
        blade.offset = { disFloat(gen) * size, yPos * size };
        blade.bladeId = newBlade;
        blade.rotation = disRot(gen);
        blade.palette = grassAssets->blades[newBlade].averageColor;
        blades.push_back(blade);
	}

    std::sort(blades.begin(), blades.end(), [](const BladeData& a, const BladeData& b) {
        return a.bladeId < b.bladeId;
        });
}

void GrassTile::ApplyForce(Vector2 position, float radius, float dropoff) {
    for (auto& blade : blades) {
        Vector2 bladePos = { location.x + blade.offset.x, location.y + blade.offset.y };
        float distance = Vector2Distance(bladePos, position);

        if (distance < radius) {
            blade.rotation += 90 * (position.x > bladePos.x ? 1 : -1);
        }
        else if (distance < radius + dropoff) {
            float t = (distance - radius) / dropoff;
            blade.rotation += 90 * (1 - t) * (position.x > bladePos.x ? 1 : -1);
        }
    }
}
