#include "GrassAssets.hpp"
#include <map>
#include "GrassTile.hpp"
#include <functional>
class GrassManager {
public:
	GrassAssets* ga;
	GrassConfiguration gc;
	// Grass tiles stored by grid coordinate (x,y)
	std::map<std::pair<int, int>, GrassTile*> grassTiles = {};
	GrassManager(const std::string& grassPath, int tileSize = 16, int shadeAmount = 100,
		int stiffness = 360, int maxUnique = 10, const int verticalPlaceRange[2] = nullptr,
		int padding = 13);
	~GrassManager();
	void enableGroundShadows(int shadowStrength = 40, int shadowRadius = 2, Color shadowColor = { 0, 0, 10, 0 }, Vector2 shadowShift = { 0, 0 });
	void burnTile(std::pair<int, int> location);
	void placeTile(std::pair<int, int> location, int density, const std::vector<int>& grassOptions);
	void applyForce(Vector2 location, float radius, float dropoff);
	void updateRender(float deltaTime, Vector2 screenSize, Vector2 offset = { 0, 0 }, std::function<int(int, int)>* rotFunction = nullptr);
};