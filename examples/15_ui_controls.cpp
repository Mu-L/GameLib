// 15_ui_controls.cpp - Basic UI Controls Demo
//
// Demonstrates the immediate-mode Button, Checkbox, RadioBox
// and ToggleButton helpers.
// Labels use the built-in 8x8 bitmap font, so keep them ASCII.
//
// Controls:
//   Mouse left button : interact with buttons and checkboxes
//   ESC               : quit
//
// Learn: Button, Checkbox, RadioBox, ToggleButton,
//        release-trigger UI, built-in 8x8 UI labels
//
// Compile (Win32): g++ -o 15_ui_controls.exe 15_ui_controls.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 15_ui_controls 15_ui_controls.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

static void DrawPanel(GameLib &game, int x, int y, int w, int h, const char *title)
{
    game.FillRect(x, y, w, h, COLOR_RGB(28, 34, 50));
    game.DrawRect(x, y, w, h, COLOR_RGB(84, 94, 120));
    game.FillRect(x + 1, y + 1, w - 2, 22, COLOR_RGB(38, 48, 72));
    game.DrawText(x + 8, y + 7, title, COLOR_WHITE);
}

static void DrawBackdrop(GameLib &game, bool showGrid)
{
    game.Clear(COLOR_RGB(18, 22, 34));

    for (int y = 0; y < game.GetHeight(); y += 40) {
        uint32_t stripe = ((y / 40) & 1) ? COLOR_RGB(20, 26, 40) : COLOR_RGB(16, 20, 32);
        game.FillRect(0, y, game.GetWidth(), 40, stripe);
    }

    if (!showGrid) return;

    uint32_t gridColor = COLOR_ARGB(70, 120, 138, 168);
    for (int x = 0; x < game.GetWidth(); x += 20)
        game.DrawLine(x, 0, x, game.GetHeight() - 1, gridColor);
    for (int y = 0; y < game.GetHeight(); y += 20)
        game.DrawLine(0, y, game.GetWidth() - 1, y, gridColor);
}

int main()
{
    GameLib game;
    game.Open(960, 520, "15 - UI Controls", true);

    bool musicOn = true, sfxOn = true, showGrid = false, hardMode = false;
    int difficulty = 0;
    bool paused = false, turbo = false;
    int startCount = 0, resetCount = 0;
    const char *lastEvent = "NONE";

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        DrawBackdrop(game, showGrid);

        game.FillRect(0, 0, 960, 56, COLOR_RGB(10, 14, 24));
        game.DrawTextScale(20, 14, "UI CONTROLS", COLOR_WHITE, 2);
        game.DrawText(20, 40, "Mouse: press inside, release inside to trigger. ESC quits.", COLOR_LIGHT_GRAY);

        DrawPanel(game, 20, 76, 184, 420, "Buttons");
        DrawPanel(game, 224, 76, 184, 420, "Checkboxes");
        DrawPanel(game, 428, 76, 184, 420, "RadioBox");
        DrawPanel(game, 632, 76, 184, 420, "Toggle");
        DrawPanel(game, 836, 76, 104, 420, "Status");

        if (game.Button(40, 116, 144, 32, "START", COLOR_RGB(52, 150, 92))) {
            startCount++; lastEvent = "START";
        }
        if (game.Button(40, 160, 144, 32, "RESET", COLOR_RGB(196, 142, 46))) {
            musicOn = true; sfxOn = true; showGrid = false; hardMode = false;
            resetCount++; lastEvent = "RESET";
        }
        if (game.Button(40, 204, 144, 32, "QUIT", COLOR_RGB(180, 76, 76))) break;

        game.DrawText(40, 264, "The button label uses", COLOR_LIGHT_GRAY);
        game.DrawText(40, 280, "the built-in 8x8 font.", COLOR_LIGHT_GRAY);
        game.DrawText(40, 320, "Visual states:", COLOR_WHITE);
        game.DrawText(40, 340, "NORMAL / HOVER / PRESSED", COLOR_LIGHT_GRAY);

        if (game.Checkbox(244, 116, "MUSIC", &musicOn))
            lastEvent = musicOn ? "MUSIC ON" : "MUSIC OFF";
        if (game.Checkbox(244, 152, "SFX", &sfxOn))
            lastEvent = sfxOn ? "SFX ON" : "SFX OFF";
        if (game.Checkbox(244, 188, "SHOW GRID", &showGrid))
            lastEvent = showGrid ? "GRID ON" : "GRID OFF";
        if (game.Checkbox(244, 224, "HARD MODE", &hardMode))
            lastEvent = hardMode ? "HARD ON" : "HARD OFF";

        game.DrawText(244, 276, "Click covers box", COLOR_WHITE);
        game.DrawText(244, 292, "and label.", COLOR_LIGHT_GRAY);
        game.DrawText(244, 324, "4 states:", COLOR_WHITE);
        game.DrawText(244, 340, "CHK/UNCHK", COLOR_LIGHT_GRAY);
        game.DrawText(244, 356, "+ hover.", COLOR_LIGHT_GRAY);

        if (game.RadioBox(448, 116, "EASY", &difficulty, 0)) lastEvent = "EASY";
        if (game.RadioBox(448, 152, "MEDIUM", &difficulty, 1)) lastEvent = "MEDIUM";
        if (game.RadioBox(448, 188, "HARD", &difficulty, 2)) lastEvent = "HARD";

        game.DrawText(448, 232, "Same group shares", COLOR_WHITE);
        game.DrawText(448, 248, "one int *value.", COLOR_LIGHT_GRAY);
        game.DrawText(448, 280, "Selected:", COLOR_WHITE);
        const char *diffNames[] = {"EASY", "MEDIUM", "HARD"};
        game.DrawText(448, 296, diffNames[difficulty], COLOR_YELLOW);
        game.DrawText(448, 336, "Circle + dot", COLOR_WHITE);
        game.DrawText(448, 352, "instead of box.", COLOR_LIGHT_GRAY);

        if (game.ToggleButton(652, 116, 144, 32, "PAUSE", &paused, COLOR_RGB(180, 76, 76)))
            lastEvent = paused ? "PAUSED" : "RESUME";
        if (game.ToggleButton(652, 160, 144, 32, "TURBO", &turbo, COLOR_RGB(52, 150, 92)))
            lastEvent = turbo ? "TURBO ON" : "TURBO OFF";

        game.DrawText(652, 216, "Toggled=ON shows", COLOR_WHITE);
        game.DrawText(652, 232, "sunken bevel.", COLOR_LIGHT_GRAY);
        game.DrawText(652, 264, "PAUSED:", COLOR_WHITE);
        game.DrawText(652, 280, paused ? "YES" : "NO", paused ? COLOR_YELLOW : COLOR_LIGHT_GRAY);
        game.DrawText(652, 312, "TURBO:", COLOR_WHITE);
        game.DrawText(652, 328, turbo ? "YES" : "NO", turbo ? COLOR_YELLOW : COLOR_LIGHT_GRAY);

        game.DrawText(856, 116, "LAST EVENT", COLOR_WHITE);
        game.DrawText(856, 136, lastEvent, COLOR_YELLOW);
        game.DrawText(856, 176, "COUNTS", COLOR_WHITE);
        game.DrawPrintf(856, 196, COLOR_LIGHT_GRAY, "START: %d", startCount);
        game.DrawPrintf(856, 212, COLOR_LIGHT_GRAY, "RESET: %d", resetCount);

        game.DrawText(856, 252, "FLAGS", COLOR_WHITE);
        game.DrawPrintf(856, 272, COLOR_LIGHT_GRAY, "MUSIC: %s", musicOn ? "ON" : "OFF");
        game.DrawPrintf(856, 288, COLOR_LIGHT_GRAY, "SFX: %s", sfxOn ? "ON" : "OFF");
        game.DrawPrintf(856, 304, COLOR_LIGHT_GRAY, "GRID: %s", showGrid ? "ON" : "OFF");
        game.DrawPrintf(856, 320, COLOR_LIGHT_GRAY, "HARD: %s", hardMode ? "ON" : "OFF");

        game.DrawText(856, 352, "DIFF:", COLOR_WHITE);
        game.DrawText(856, 368, diffNames[difficulty], COLOR_YELLOW);
        game.DrawPrintf(856, 384, COLOR_LIGHT_GRAY, "PAUSE:%s", paused ? "Y" : "N");
        game.DrawPrintf(856, 400, COLOR_LIGHT_GRAY, "TURBO:%s", turbo ? "Y" : "N");

        game.Update();
        game.WaitFrame(60);
    }

    return 0;
}
