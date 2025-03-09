#pragma once
#include <raylib.h>
#include <vector>
#include "Utils.hpp"
#include "GrassAssets.hpp"
class GrassTile {
public:
	float burnLife = 30.0;
	float maxBurnLife = 30.0;
	float burning = 60.0f;
	float masterRotation = 0.0f;
	float precision = 30.0f;
	float inc = 3.0f;
	int baseId = 0;
	float trueRotation = 0.0f;
	std::vector<BladeData> blades;
	std::vector<BladeData> custom_blade_data;
	GrassAssets* ga;
	GrassConfiguration grassConfiguration;
	Vector2 location;
	float size; 
	GrassTile(int tileSize, Vector2 location, int amount, const std::vector<int>& grassOptions, GrassConfiguration grassConfiguration, GrassAssets* grassAssets);	
	void ApplyForce(Vector2 position, float radius, float dropoff);
	void RenderShadow(Vector2 offset);
	void Render(Vector2 offset);
	void UpdateRenderData(float dt);
	void SetRotation(float rotation, float dt);
};