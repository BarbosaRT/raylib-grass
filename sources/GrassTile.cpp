#include "GrassTile.hpp"
#include <random>
#include <raymath.h>
#include <iostream>
GrassTile::GrassTile(int tileSize, Vector2 location, int amount, const std::vector<int>& grassOptions, GrassConfiguration gc, GrassAssets* grassAssets) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disFloat(0.0f, 1.0f);
    std::uniform_real_distribution<float> disRot(-15.0f, 15.0f);
    size = tileSize;
    this->location = location;
    grassConfiguration = gc;
    ga = grassAssets;

    float range = grassConfiguration.verticalPlaceRange[1] - grassConfiguration.verticalPlaceRange[0];
	for (int i = 0; i < amount; i++) {
        int newBlade = grassOptions[rand() % grassOptions.size()];
        float yPos = grassConfiguration.verticalPlaceRange[0];
        if (range > 0) {
            yPos = disFloat(gen) * range + grassConfiguration.verticalPlaceRange[0];
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

    UpdateRenderData(0);
}

void GrassTile::ApplyForce(Vector2 position, float radius, float dropoff) {
    if (custom_blade_data.empty()) {
        custom_blade_data = std::vector<BladeData>(blades.size());
    }

    for (size_t i = 0; i < blades.size(); ++i) {
        BladeData& blade = blades[i];
        Vector2 bladePos = { location.x + blade.offset.x, location.y + blade.offset.y };
        float distance = Vector2Distance(bladePos, position);
        float force = 0.0f;
        float dir = (position.x > bladePos.x) ? -1.0f : 1.0f;

        if (distance < radius) {
            force = 1.0f;
        }
        else if (distance < radius + dropoff) {
            float t = (distance - radius) / dropoff;
            force = 1.0f - t;
        }

        if (custom_blade_data[i].rotation == 0.0f || std::abs(custom_blade_data[i].rotation - blade.rotation) <= std::abs(force) * 90)
        {
            custom_blade_data[i] = blade; //copy the blade
            custom_blade_data[i].rotation += dir * force * 90;
        }
    }

    /*for (auto& blade : blades) {
        Vector2 bladePos = { location.x + blade.offset.x, location.y + blade.offset.y };
        float distance = Vector2Distance(bladePos, position);

        if (distance < radius) {
            blade.rotation += 90 * (position.x > bladePos.x ? 1 : -1);
        }
        else if (distance < radius + dropoff) {
            float t = (distance - radius) / dropoff;
            blade.rotation += 90 * (1 - t) * (position.x > bladePos.x ? 1 : -1);
        }
    }*/
}

// Set a new master rotation and update render data.
void GrassTile::SetRotation(float rotation, float dt) {
    masterRotation = rotation;
    UpdateRenderData(dt);
}

void GrassTile::RenderShadow(Vector2 offset)
{
    if (grassConfiguration.groundShadow.radius > 0)
    {
        for (const auto& blade : blades) {
            Vector2 shadowPos = { location.x + blade.offset.x + grassConfiguration.padding - offset.x,
                                      location.y + blade.offset.y + grassConfiguration.padding - offset.y };
            DrawCircle(shadowPos.x, shadowPos.y, grassConfiguration.groundShadow.radius,
                Fade(grassConfiguration.groundShadow.color, (float)grassConfiguration.groundShadow.strength / 255.0f)); //strength is 0-100, Fade expects 0-1
        }
    }
}

void GrassTile::Render(Vector2 offset)
{
    //Use blades, or custom_blade_data if it is not empty.
    const std::vector<BladeData>& bladesToRender = custom_blade_data.empty() ? blades : custom_blade_data;

    //std::cout << burnLife / maxBurnLife << "\n";
    for (const auto& blade : bladesToRender) {
        Vector2 bladePos = { location.x + blade.offset.x,
                             location.y + blade.offset.y };
        ga->renderBlade(blade.bladeId, bladePos, blade.rotation + trueRotation, burnLife / maxBurnLife, blade.palette); //pass the correct scale.
    }
}

void GrassTile::UpdateRenderData(float dt)
{
    if (burning == 0) {
        burnLife = std::max(0.0f, burnLife - 25 * dt);
    }
    else {
        trueRotation = inc * masterRotation;
    }

    if (!custom_blade_data.empty()) { //only normalize when we have applied custom forces
        bool matching = true;
        for (size_t i = 0; i < blades.size(); ++i) {
            custom_blade_data[i].rotation = normalize(custom_blade_data[i].rotation, grassConfiguration.stiffness * dt, blades[i].rotation);
            if (custom_blade_data[i].rotation != blades[i].rotation) {
                matching = false;
            }
        }
        if (matching) {
            custom_blade_data.clear(); // Go back to the "base" state
        }
    }
}
