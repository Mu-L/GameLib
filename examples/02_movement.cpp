// 02_movement.cpp - Movement and Physics
//
// Two modes (press TAB to switch):
//   Mode A: Arrow keys move a box around the screen.
//   Mode B: A bouncing ball with a fading trail.
// Learn: IsKeyDown, FillRect, FillCircle, DrawCircle, DrawPrintf,
//        GetFPS, GetDeltaTime, float physics, wall collision
//
// Win32: g++ -o 02_movement.exe 02_movement.cpp -mwindows
// SDL:   g++ -DUSE_SDL -std=c++11 -O2 -o 02_movement.exe 02_movement.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

int main()
{
    GameLib game;
    game.Open(640, 480, "02 - Movement and Physics", true);

    // --- Mode A: Keyboard controlled box ---
    float boxX = 310.0f, boxY = 230.0f;
    int boxSize = 20;
    float boxSpeed = 200.0f; // pixels per second

    // --- Mode B: Bouncing ball ---
    float ballX = 320.0f, ballY = 240.0f;
    float ballVX = 240.0f, ballVY = 180.0f; // pixels per second
    int ballR = 20;

    // Trail for bouncing ball
    float trailX[64], trailY[64];
    int trailCount = 0;

    bool showBall = false; // false = mode A (box), true = mode B (ball)

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;
        if (game.IsKeyPressed(KEY_TAB)) showBall = !showBall;

        float dt = (float)game.GetDeltaTime();
        if (dt > 0.05f) dt = 0.05f; // prevent first-frame jump

        if (!showBall) {
            // --- Mode A: Keyboard Control ---
            if (game.IsKeyDown(KEY_LEFT))  boxX -= boxSpeed * dt;
            if (game.IsKeyDown(KEY_RIGHT)) boxX += boxSpeed * dt;
            if (game.IsKeyDown(KEY_UP))    boxY -= boxSpeed * dt;
            if (game.IsKeyDown(KEY_DOWN))  boxY += boxSpeed * dt;

            // Keep inside window bounds
            if (boxX < 0) boxX = 0;
            if (boxY < 0) boxY = 0;
            if (boxX + boxSize > game.GetWidth())  boxX = (float)(game.GetWidth() - boxSize);
            if (boxY + boxSize > game.GetHeight()) boxY = (float)(game.GetHeight() - boxSize);
        } else {
            // --- Mode B: Bouncing Ball ---
            ballX += ballVX * dt;
            ballY += ballVY * dt;

            if (ballX - ballR < 0)                { ballX = (float)ballR; ballVX = -ballVX; }
            if (ballX + ballR > game.GetWidth())   { ballX = (float)(game.GetWidth() - ballR); ballVX = -ballVX; }
            if (ballY - ballR < 0)                { ballY = (float)ballR; ballVY = -ballVY; }
            if (ballY + ballR > game.GetHeight()) { ballY = (float)(game.GetHeight() - ballR); ballVY = -ballVY; }

            // Record trail
            if (trailCount < 64) {
                trailX[trailCount] = ballX;
                trailY[trailCount] = ballY;
                trailCount++;
            } else {
                for (int i = 0; i < 63; i++) {
                    trailX[i] = trailX[i + 1];
                    trailY[i] = trailY[i + 1];
                }
                trailX[63] = ballX;
                trailY[63] = ballY;
            }
        }

        // --- Drawing ---
        game.Clear(COLOR_BLACK);

        if (!showBall) {
            // Mode A drawing
            game.FillRect((int)boxX, (int)boxY, boxSize, boxSize, COLOR_CYAN);
            game.DrawText(10, 10, "Mode A: Arrow keys to move", COLOR_WHITE);
            game.DrawPrintf(10, 25, COLOR_GRAY, "Position: %.0f, %.0f", boxX, boxY);
        } else {
            // Mode B drawing: trail
            for (int i = 0; i < trailCount; i++) {
                int brightness = 40 + i * 3;
                if (brightness > 255) brightness = 255;
                uint32_t c = COLOR_RGB(brightness, 0, 0);
                int tr = 2 + i * ballR / 64;
                game.FillCircle((int)trailX[i], (int)trailY[i], tr, c);
            }
            // Ball
            game.FillCircle((int)ballX, (int)ballY, ballR, COLOR_RED);
            game.DrawCircle((int)ballX, (int)ballY, ballR, COLOR_WHITE);
            game.DrawText(10, 10, "Mode B: Bouncing Ball", COLOR_WHITE);
            game.DrawPrintf(10, 25, COLOR_GRAY, "Ball: %.0f, %.0f  Speed: %.0f, %.0f", ballX, ballY, ballVX, ballVY);
        }

        // Common HUD
        game.DrawPrintf(10, 460, COLOR_LIGHT_GRAY, "FPS: %.0f  |  TAB: Switch Mode  |  ESC: Quit", game.GetFPS());

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
