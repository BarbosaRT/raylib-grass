#include "GrassManager.hpp"
#include <iostream>
GrassManager::GrassManager(const std::string& grassPath, int tileSize, int shadeAmount, int stiffness, int maxUnique, const int verticalPlaceRange[2], int padding)
{
	gc.shadeAmount = 10;
	gc.tileSize = tileSize;
	gc.shadeAmount = shadeAmount;
	gc.stiffness = stiffness;
	gc.maxUnique = maxUnique;

	if (verticalPlaceRange != nullptr) {
		gc.verticalPlaceRange[0] = verticalPlaceRange[0];
		gc.verticalPlaceRange[1] = verticalPlaceRange[1];
	}
	else {
		gc.verticalPlaceRange[0] = 1; // Default values
		gc.verticalPlaceRange[1] = 1;
	}

	GroundShadow groundShadow;
	groundShadow.strength = 0;
	groundShadow.color = BLACK;
	groundShadow.radius = 100;
	groundShadow.shift = Vector2(0, 0);
	gc.groundShadow = groundShadow;
	gc.padding = padding;

	ga = new GrassAssets(grassPath, gc.shadeAmount);
}

GrassManager::~GrassManager() {
	delete ga;
	// Clean up GrassTile pointers
	for (auto& pair : grassTiles) {
		delete pair.second;
	}
	grassTiles.clear();
}

void GrassManager::enableGroundShadows(int shadowStrength, int shadowRadius, Color shadowColor, Vector2 shadowShift)
{
	GroundShadow groundShadow;
	groundShadow.strength = shadowStrength;
	groundShadow.color = shadowColor;
	groundShadow.radius = shadowRadius;
	groundShadow.shift = shadowShift;
	gc.groundShadow = groundShadow;
}

void GrassManager::burnTile(std::pair<int, int> location) {
	if (grassTiles.count(location) != 0) {
		grassTiles[location]->burning = 0.0f;
	}
}

void GrassManager::placeTile(std::pair<int, int> location, int density, const std::vector<int>& grassOptions)
{
	if (grassTiles.count(location) == 0) {
		Vector2 position = { location.first * gc.tileSize, location.second * gc.tileSize };
		grassTiles[location] = new GrassTile(gc.tileSize, position, density, grassOptions, gc, ga);
	}
}

void GrassManager::applyForce(Vector2 location, float radius, float dropoff) {
	int gridX = (int)(location.x / gc.tileSize);
	int gridY = (int)(location.y / gc.tileSize);
	int tileRange = (int)ceil((radius + dropoff) / gc.tileSize);

	for (int y = -tileRange; y <= tileRange; y++) {
		for (int x = -tileRange; x <= tileRange; x++) {
			std::pair<int, int> pos = { gridX + x, gridY + y };
			if (grassTiles.find(pos) != grassTiles.end())
				grassTiles[pos]->ApplyForce(location, radius, dropoff);
		}
	}
}

void GrassManager::updateRender(float deltaTime, Vector2 screenSize, Vector2 offset, std::function<int(int, int)>* rotFunction)
{
	// Calculate visible rectangle in WORLD coordinates
	Rectangle visibleRectWorld = {
		offset.x - screenSize.x / 2 + gc.padding,  // Center around camera target
		offset.y - screenSize.y / 2 + gc.padding,
		screenSize.x + 2 * gc.padding,
		screenSize.y + 2 * gc.padding
	};

	// Optimized tile iteration:
	int startTileX = (int)((visibleRectWorld.x) / gc.tileSize);
	int startTileY = (int)((visibleRectWorld.y) / gc.tileSize);
	int endTileX = (int)((visibleRectWorld.x + visibleRectWorld.width) / gc.tileSize);
	int endTileY = (int)((visibleRectWorld.y + visibleRectWorld.height) / gc.tileSize);
	std::vector<std::pair<int, int>> renderList; //we will store the tiles to be rendered.

	for (int y = startTileY; y <= endTileY; ++y) {
		for (int x = startTileX; x <= endTileX; ++x) {
			std::pair<int, int> pos = { x, y };
			if (grassTiles.count(pos)) {
				renderList.push_back(pos); // Only add if the tile exists
			}
		}
	}

	// Render shadows (if enabled)
	if (gc.groundShadow.radius > 0) {
		for (const auto& pos : renderList) {
			GrassTile* tile = grassTiles[pos];
			// Call a modified RenderShadow function
			tile->RenderShadow({ offset.x - gc.groundShadow.shift.x, offset.y - gc.groundShadow.shift.y });
		}
	}

	// Render grass tiles and handle burning
	for (const auto& pos : renderList) {
		GrassTile* tile = grassTiles[pos];
		bool markedForDeletion = false;
		if (tile->burning == 0) {
			// Burn spread logic
			for (const auto& offset_ : BURN_CHECK_OFFSETS) {
				std::pair<int, int> check_pos = { pos.first + offset_.x, pos.second + offset_.y };
				if (grassTiles.find(check_pos) != grassTiles.end()) {
					grassTiles[check_pos]->burning = std::max(0.0f, grassTiles[check_pos]->burning - 1);
				}
			}
			markedForDeletion = tile->burnLife <= 0;
		}
		// Render the tile
		tile->Render(offset);

		// Apply the rotation function if provided
		if (rotFunction) {
			// Dereference the function pointer and call the function
			float rotation = (*rotFunction)(tile->location.x, tile->location.y);
			tile->SetRotation(rotation, deltaTime);
		}

		if (markedForDeletion) {
			tile->blades.clear();
			grassTiles.erase(pos);
			delete tile;
		}
	}
}

