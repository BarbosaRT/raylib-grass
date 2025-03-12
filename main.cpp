#include <raylib.h>
#include <raymath.h> // For Vector2 operations
#include "sources/GrassManager.hpp"
#include <iostream>
#include <vector>
#include <random>

int main(void) {
    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const int displayWidth = 640;
    const int displayHeight = 360;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Grass Demo");

    // Camera setup
    Camera2D camera = { 0 };
    camera.zoom = screenWidth / displayWidth;
    camera.offset = { displayWidth / 2, displayHeight / 2};  // Centering correctly
    camera.target = { 0, 0 };  // Initialized properly

    // Grass Manager initialization
    int tileSize = 10;
    GrassManager grassManager("assets/grass", tileSize, 69, 600, 5, new int[2] {0, 1}, 69);
    //grassManager.enableGroundShadows(10, 5, { 0, 0, 1, 255 }, { 1, 2 });
    grassManager.enableGroundShadows(0, 0, { 0, 0, 1, 255 }, { 1, 2 }); // Shadows Disabled

    // Initial grass placement
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    for (int y = 5; y < 25; ++y) {
        for (int x = 5; x < 25; ++x) {
            float v = dis(gen);
            if (v > 0.1f) {
                std::vector<int> bladeIds = { 0, 1, 2, 3, 4 };
                grassManager.placeTile({ x, y }, (int)(v * 12), bladeIds);
            }
        }
    }

    // Variables
    float t = 0.0f;
    Vector2 scroll = { 0.0f, 0.0f }; // Camera target in world coordinates
    float cameraSpeed = 100.0f;
    bool clicking = false;
    float brushSize = 1.0f;
    bool burn = false;

    SetTargetFPS(1000);

    // Main game loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- Input Handling ---
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, camera); // Correct world position

        // Camera movement (use correct proportions)
        if (mousePos.x / screenWidth < 0.2f)  scroll.x -= (cameraSpeed / camera.zoom) * dt;
        if (mousePos.x / screenWidth > 0.8f)  scroll.x += (cameraSpeed / camera.zoom) * dt;
        if (mousePos.y / screenHeight < 0.2f) scroll.y -= (cameraSpeed / camera.zoom) * dt;
        if (mousePos.y / screenHeight > 0.8f) scroll.y += (cameraSpeed / camera.zoom) * dt;

        // Update camera target
        camera.target = scroll;

        // Apply force
        grassManager.applyForce(worldMousePos, 10 * brushSize, 25 * brushSize);

        // Wind rotation function
        std::function<int(int, int)>* rotFunction = new std::function<int(int, int)>(
            [&](int x, int y) -> int {
                return (int)(sin(t / 60.0f + x / 100.0f) * 15);
            }
        );

        // --- Event Handling ---
        if (IsKeyPressed(KEY_B)) burn = !burn;
        if (IsKeyPressed(KEY_ESCAPE)) break;

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            brushSize = std::min(1.0f, brushSize + (4.0f * dt));
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            brushSize = std::max(0.1f, brushSize - (4.0f * dt));
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) clicking = true;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) clicking = false;

        // --- Update ---
        t += dt * 100.0f;

        // Place/burn tiles
        if (clicking) {
            int tileX = (int)(worldMousePos.x / grassManager.gc.tileSize);
            int tileY = (int)(worldMousePos.y / grassManager.gc.tileSize);
            std::vector<int> bladeIds = { 0, 1, 2, 3, 5 };
            if (!burn) {
                grassManager.placeTile({ tileX, tileY }, (int)(dis(gen) * 12 * brushSize + 1), bladeIds);
                if (brushSize == 1.0f) {
                    std::vector<std::pair<int, int>> offsets = {
                        {-1, 0}, {-1, -1}, {0, -1}, {1, -1},
                        {1, 0}, {1, 1}, {0, 1}, {-1, 1}
                    };
                    for (const auto& offset : offsets) {
                        grassManager.placeTile({ tileX + offset.first, tileY + offset.second }, (int)(dis(gen) * 14 + 3), bladeIds);
                    }
                }
            }
            else {
                grassManager.burnTile({ tileX, tileY });
            }
        }

        // --- Render ---
        BeginDrawing();
        ClearBackground({ 27, 66, 52, 255 });

        BeginMode2D(camera);

        // Draw grass (world coordinates)
        grassManager.updateRender(dt, {displayWidth, displayHeight }, camera.target, rotFunction);
        // Draw mouse circle (world coordinates)
        DrawCircleV(worldMousePos, 10 * brushSize - (clicking ? 2 : 0),
            Fade(WHITE, clicking ? 1.0f : 0.5f));
        if (clicking) {
            DrawCircleLinesV(worldMousePos, 10 * brushSize, WHITE);
        }

        EndMode2D();
        //DrawFPS(10, 10);
        EndDrawing();

        delete rotFunction;
    }

    CloseWindow();
    return 0;
}
