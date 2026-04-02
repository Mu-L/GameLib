// 01_hello.cpp - Hello World
//
// The simplest GameLib program: create a window and display text.
// Learn: Open, Clear, DrawText, DrawTextScale, Update, WaitFrame, IsClosed
//
// Compile: g++ -o 01_hello.exe 01_hello.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "01 - Hello World", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_DARK_BLUE);

        // Large title
        game.DrawTextScale(160, 180, "Hello, GameLib!", COLOR_GOLD, 3);

        // Normal text
        game.DrawText(230, 240, "Welcome to GameLib!", COLOR_WHITE);
        game.DrawText(210, 270, "Press ESC to exit, or close", COLOR_GRAY);
        game.DrawText(210, 280, "the window to quit.", COLOR_GRAY);

        // Show running time
        game.DrawPrintf(10, 460, COLOR_LIGHT_GRAY, "Time: %.1f s", game.GetTime());

        if (game.IsKeyPressed(KEY_ESCAPE))
            break;

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
