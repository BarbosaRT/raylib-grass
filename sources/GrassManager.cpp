#include "GrassManager.hpp"

GrassManager::GrassManager(const std::string& grassPath, int tileSize, int shadeAmount, int stiffness, int maxUnique, const int verticalPlaceRange[2], int padding)
{
	gc.shadeAmount = 10;
	gc.tileSize = tileSize;
	gc.shadeAmount = shadeAmount;
	gc.stiffness = stiffness;
	gc.maxUnique = maxUnique;
	gc.verticalPlaceRange[0] = verticalPlaceRange[0];
	gc.verticalPlaceRange[1] = verticalPlaceRange[1];

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
	delete ga;  // Desaloca a memória de GrassAssets
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
	if (grassTiles.at(location)) {
		grassTiles[location]->burning = 0;
	}
}

void GrassManager::placeTile(std::pair<int, int> location, int density, const std::vector<int>& grassOptions)
{
	if (!grassTiles.at(location)) {
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
	// Calculate the visible tile range
	int visible_tile_range_x = static_cast<int>(screenSize.x / gc.tileSize) + 1;
	int visible_tile_range_y = static_cast<int>(screenSize.y / gc.tileSize) + 1;
	Vector2 base_pos = { offset.x / gc.tileSize, offset.y / gc.tileSize };

	std::vector<std::pair<int, int>> render_list;
	for (int y = 0; y < visible_tile_range_y; ++y) {
		for (int x = 0; x < visible_tile_range_x; ++x) {
			std::pair<int, int> pos = { base_pos.x + x, base_pos.y + y };
			if (grassTiles.find(pos) != grassTiles.end()) {
				render_list.push_back(pos);
			}
		}
	}

	// Render shadow if applicable
	//if (gc.groundShadow.strength) {
	//	for (const auto& pos : render_list) {
	//		grassTiles[pos]->renderShadow(surf, { offset.x - ground_shadow.second, offset.y - ground_shadow.second });
	//	}
	//}

	// Render grass tiles and handle burning
	for (const auto& pos : render_list) {
		GrassTile* tile = grassTiles[pos];
		if (tile->burning == 0) {
			// Burn spread logic
			for (const auto& offset_ : BURN_CHECK_OFFSETS) {
				std::pair<int, int> check_pos = { pos.first + offset_.x, pos.second + offset_.y };
				if (grassTiles.find(check_pos) != grassTiles.end()) {
					grassTiles[check_pos]->burning = std::max(0.0f, grassTiles[check_pos]->burning - 1);
				}
			}
		}

		// Render the tile
		//tile->Render(offset);

		// Apply the rotation function if provided
		if (rotFunction) {
			// Dereference the function pointer and call the function
			float rotation = (*rotFunction)(tile->location.x, tile->location.y);
			//tile->SetRotation(rotation, deltaTime);
		}
	}
}
