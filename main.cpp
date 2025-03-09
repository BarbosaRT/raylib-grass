#include <raylib.h>
#include <raymath.h> // For Vector2 operations
#include "sources/GrassManager.hpp"
#include <iostream>
#include <vector>
#include <random>

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 600;  // Match Pygame example
    const int screenHeight = 600;
    const int displayWidth = 300;
    const int displayHeight = 300;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Good practice
    InitWindow(screenWidth, screenHeight, "Grass Demo");

    // Camera setup
    Camera2D camera = { 0 };
    camera.zoom = 2.0f; // Start zoomed in (to match display size). This is correct.
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f }; // Correct: Center of the *window*
    // camera.target is initialized to {0,0} and will be updated in the loop

    // Grass Manager initialization
    GrassManager grassManager("assets/grass", 10, 100, 600, 5, new int[2] {0, 1}, 13); // Use dynamic array
    grassManager.enableGroundShadows(4, 1, { 0, 0, 1, 255 }, { 1, 2 });


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
    Vector2 scroll = { 0.0f, 0.0f }; // This represents the camera's *target* (world coordinates)
    float cameraSpeed = 170.0f * 2.0f; // Adjust speed, multiplied by two because of the zoom.
    bool clicking = false;
    float brushSize = 1.0f;
    bool burn = false;

    SetTargetFPS(1000); // Match the demo
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- Input Handling ---

        // Get *screen* mouse position (no scaling needed!)
        Vector2 mousePos = GetMousePosition();
        // Convert mouse position to world coordinates
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, camera);

        // Camera movement (control the *target*)
        if (mousePos.x / screenWidth < 0.2f)  scroll.x -= cameraSpeed * dt;
        if (mousePos.x / screenWidth > 0.8f)  scroll.x += cameraSpeed * dt;
        if (mousePos.y / screenHeight < 0.2f) scroll.y -= cameraSpeed * dt;
        if (mousePos.y / screenHeight > 0.8f) scroll.y += cameraSpeed * dt;

        // Update camera target
        camera.target = scroll;  // This is the *correct* way to control camera movement

        // Apply force (use world coordinates)
        grassManager.applyForce(worldMousePos, 10 * brushSize, 25 * brushSize);


        // Wind rotation function (using a lambda)
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

        // Place/burn tiles (use world coordinates)
        if (clicking) {
            int tileX = (int)(worldMousePos.x / grassManager.gc.tileSize);
            int tileY = (int)(worldMousePos.y / grassManager.gc.tileSize);
            std::vector<int> bladeIds = { 0, 1, 2, 3, 4 };
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

        BeginMode2D(camera); // Essential for camera transformations

        // Draw grass (using world coordinates, handled by updateRender)
        grassManager.updateRender(dt, { (float)displayWidth, (float)displayHeight }, { 0,0 }, rotFunction);

        // Draw mouse circle (use *world* coordinates)
        DrawCircleV(worldMousePos, 10 * brushSize - (clicking ? 2 : 0),
            Fade(WHITE, clicking ? 1.0f : 0.5f));
        if (clicking) {
            DrawCircleLinesV(worldMousePos, 10 * brushSize, WHITE);
        }

        EndMode2D(); // Essential

        DrawFPS(10, 10);
        EndDrawing();
        delete rotFunction;
    }

    CloseWindow();
    return 0;
}