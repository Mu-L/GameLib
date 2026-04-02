// 04_bouncing_ball.cpp - Bouncing Ball
//
// A bouncing ball that bounces off the walls.
// Learn: float movement, velocity/direction, wall collision, DrawPrintf, GetFPS
//
// Compile: g++ -o 04_bouncing_ball.exe 04_bouncing_ball.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "04 - Bouncing Ball", true);

    float x = 320, y = 240;
    float vx = 4.0f, vy = 3.0f;
    int r = 20;

    // History positions for drawing trail
    float trailX[64], trailY[64];
    int trailCount = 0;

    while (!game.IsClosed()) {
        // Move
        x += vx;
        y += vy;

        // Bounce off walls
        if (x - r < 0)                { x = (float)r; vx = -vx; }
        if (x + r > game.GetWidth())   { x = (float)(game.GetWidth() - r); vx = -vx; }
        if (y - r < 0)                { y = (float)r; vy = -vy; }
        if (y + r > game.GetHeight()) { y = (float)(game.GetHeight() - r); vy = -vy; }

        // Record trail
        if (trailCount < 64) {
            trailX[trailCount] = x;
            trailY[trailCount] = y;
            trailCount++;
        } else {
            for (int i = 0; i < 63; i++) {
                trailX[i] = trailX[i + 1];
                trailY[i] = trailY[i + 1];
            }
            trailX[63] = x;
            trailY[63] = y;
        }

        // Draw
        game.Clear(COLOR_BLACK);

        // Trail (older = darker)
        for (int i = 0; i < trailCount; i++) {
            int brightness = 40 + i * 3;
            if (brightness > 255) brightness = 255;
            uint32_t c = COLOR_RGB(brightness, 0, 0);
            int tr = 2 + i * r / 64;
            game.FillCircle((int)trailX[i], (int)trailY[i], tr, c);
        }

        // Ball
        game.FillCircle((int)x, (int)y, r, COLOR_RED);
        game.DrawCircle((int)x, (int)y, r, COLOR_WHITE);

        // Info
        game.DrawPrintf(10, 10, COLOR_WHITE, "FPS: %.0f", game.GetFPS());
        game.DrawPrintf(10, 25, COLOR_GRAY, "Ball: %.0f, %.0f  Speed: %.1f, %.1f", x, y, vx, vy);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
