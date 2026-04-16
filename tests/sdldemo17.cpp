#define GAMELIB_SDL_DISABLE_IMAGE 1
#define GAMELIB_SDL_DISABLE_TTF 1
#define GAMELIB_SDL_DISABLE_MIXER 1
#include "../GameLib.SDL.h"

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
    for (int x = 0; x < game.GetWidth(); x += 20) {
        game.DrawLine(x, 0, x, game.GetHeight() - 1, gridColor);
    }
    for (int y = 0; y < game.GetHeight(); y += 20) {
        game.DrawLine(0, y, game.GetWidth() - 1, y, gridColor);
    }
}

int main()
{
    GameLib game;
    if (game.Open(760, 520, "SDL Demo 17 - UI Controls", true) != 0) {
        return 1;
    }

    bool musicOn = true;
    bool sfxOn = true;
    bool showGrid = false;
    bool hardMode = false;
    int startCount = 0;
    int resetCount = 0;
    const char *lastEvent = "NONE";

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        DrawBackdrop(game, showGrid);

        game.FillRect(0, 0, 760, 56, COLOR_RGB(10, 14, 24));
        game.DrawTextScale(20, 14, "SDL UI CONTROLS", COLOR_WHITE, 2);
        game.DrawText(20, 40, "Mouse: press inside, release inside to trigger. ESC quits.", COLOR_LIGHT_GRAY);

        DrawPanel(game, 20, 76, 184, 420, "Buttons");
        DrawPanel(game, 224, 76, 244, 420, "Checkboxes");
        DrawPanel(game, 488, 76, 252, 420, "Status");

        if (game.Button(40, 116, 144, 32, "START", COLOR_RGB(52, 150, 92))) {
            startCount++;
            lastEvent = "START";
        }
        if (game.Button(40, 160, 144, 32, "RESET", COLOR_RGB(196, 142, 46))) {
            musicOn = true;
            sfxOn = true;
            showGrid = false;
            hardMode = false;
            resetCount++;
            lastEvent = "RESET";
        }
        if (game.Button(40, 204, 144, 32, "QUIT", COLOR_RGB(180, 76, 76))) {
            break;
        }

        game.DrawText(40, 264, "Built-in 8x8 labels", COLOR_LIGHT_GRAY);
        game.DrawText(40, 280, "stay available here too.", COLOR_LIGHT_GRAY);
        game.DrawText(40, 320, "Visual states:", COLOR_WHITE);
        game.DrawText(40, 340, "NORMAL / HOVER / PRESSED", COLOR_LIGHT_GRAY);

        if (game.Checkbox(244, 116, "MUSIC", &musicOn)) {
            lastEvent = musicOn ? "MUSIC ON" : "MUSIC OFF";
        }
        if (game.Checkbox(244, 152, "SFX", &sfxOn)) {
            lastEvent = sfxOn ? "SFX ON" : "SFX OFF";
        }
        if (game.Checkbox(244, 188, "SHOW GRID", &showGrid)) {
            lastEvent = showGrid ? "GRID ON" : "GRID OFF";
        }
        if (game.Checkbox(244, 224, "HARD MODE", &hardMode)) {
            lastEvent = hardMode ? "HARD ON" : "HARD OFF";
        }

        game.DrawText(244, 276, "Checkbox click area", COLOR_WHITE);
        game.DrawText(244, 292, "covers box and label.", COLOR_LIGHT_GRAY);
        game.DrawText(244, 332, "Stable states:", COLOR_WHITE);
        game.DrawText(244, 352, "CHECKED / UNCHECKED", COLOR_LIGHT_GRAY);
        game.DrawText(244, 368, "plus hover variants.", COLOR_LIGHT_GRAY);

        game.DrawText(508, 116, "LAST EVENT", COLOR_WHITE);
        game.DrawText(508, 136, lastEvent, COLOR_YELLOW);
        game.DrawText(508, 176, "BUTTON COUNTS", COLOR_WHITE);
        game.DrawPrintf(508, 196, COLOR_LIGHT_GRAY, "START: %d", startCount);
        game.DrawPrintf(508, 212, COLOR_LIGHT_GRAY, "RESET: %d", resetCount);

        game.DrawText(508, 252, "CURRENT FLAGS", COLOR_WHITE);
        game.DrawPrintf(508, 272, COLOR_LIGHT_GRAY, "MUSIC: %s", musicOn ? "ON" : "OFF");
        game.DrawPrintf(508, 288, COLOR_LIGHT_GRAY, "SFX: %s", sfxOn ? "ON" : "OFF");
        game.DrawPrintf(508, 304, COLOR_LIGHT_GRAY, "GRID: %s", showGrid ? "ON" : "OFF");
        game.DrawPrintf(508, 320, COLOR_LIGHT_GRAY, "HARD: %s", hardMode ? "ON" : "OFF");

        game.DrawText(508, 368, "SDL_ttf is disabled in", COLOR_WHITE);
        game.DrawText(508, 388, "this demo on purpose.", COLOR_LIGHT_GRAY);
        game.DrawText(508, 404, "Button / Checkbox still", COLOR_LIGHT_GRAY);
        game.DrawText(508, 420, "work with DrawText.", COLOR_LIGHT_GRAY);

        game.Update();
        game.WaitFrame(60);
    }

    return 0;
}