// 02_moving_box.cpp - Keyboard Control
//
// Use arrow keys to move a box around the screen.
// Learn: IsKeyDown, FillRect, DrawText, DrawPrintf
//
// Compile: g++ -o 02_moving_box.exe 02_moving_box.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "02 - Moving Box", true);

    int x = 310, y = 230;
    int size = 20;
    int speed = 4;

    while (!game.IsClosed()) {
        // Arrow key movement
        if (game.IsKeyDown(KEY_LEFT))  x -= speed;
        if (game.IsKeyDown(KEY_RIGHT)) x += speed;
        if (game.IsKeyDown(KEY_UP))    y -= speed;
        if (game.IsKeyDown(KEY_DOWN))  y += speed;

        // Keep inside window bounds
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x + size > game.GetWidth())  x = game.GetWidth() - size;
        if (y + size > game.GetHeight()) y = game.GetHeight() - size;

        game.Clear(COLOR_BLACK);

        // Draw the box
        game.FillRect(x, y, size, size, COLOR_CYAN);

        // Hint text
        game.DrawText(10, 10, "Arrow keys to move", COLOR_WHITE);
        game.DrawPrintf(10, 25, COLOR_GRAY, "Position: %d, %d", x, y);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
