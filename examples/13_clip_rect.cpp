// 13_clip_rect.cpp - Clip Rectangle Demo
//
// Demonstrates SetClip / ClearClip / GetClip to restrict drawing to sub-regions.
// Three clip windows on screen:
//   - Shape window: lines, circles, triangles clipped at the boundary
//   - Text window: scrolling text clipped by SetClip
//   - Sprite window: code-generated sprite + scaled copies clipped
//
// Learn: SetClip, ClearClip, GetClipX/Y/W/H, how all draw calls respect clip
//
// Compile (Win32): g++ -o 13_clip_rect.exe 13_clip_rect.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 13_clip_rect 13_clip_rect.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

#include <math.h>

static int CreateStarSprite(GameLib &game)
{
    int id = game.CreateSprite(16, 16);
    if (id < 0) return -1;
    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 16; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);
    // Simple star shape
    for (int i = 0; i < 16; i++) {
        game.SetSpritePixel(id, 8, i, COLOR_YELLOW);   // vertical bar
        game.SetSpritePixel(id, i, 8, COLOR_YELLOW);   // horizontal bar
        game.SetSpritePixel(id, i, i, COLOR_GOLD);     // diagonal
        game.SetSpritePixel(id, 15 - i, i, COLOR_GOLD); // other diagonal
    }
    // Bright center
    for (int dy = 6; dy <= 10; dy++)
        for (int dx = 6; dx <= 10; dx++)
            game.SetSpritePixel(id, dx, dy, COLOR_WHITE);
    return id;
}

static void DrawWindowBorder(GameLib &game, int x, int y, int w, int h,
                             const char *title, uint32_t borderColor)
{
    game.DrawRect(x - 1, y - 1, w + 2, h + 2, borderColor);
    game.DrawText(x + 6, y + 4, title, COLOR_WHITE);
}

int main()
{
    GameLib game;
    game.Open(640, 480, "13 - Clip Rectangle Demo", true);

    int star = CreateStarSprite(game);

    // Window regions
    const int W1X = 20,  W1Y = 30,  W1W = 280, W1H = 200; // Shapes
    const int W2X = 330, W2Y = 30,  W2W = 290, W2H = 200; // Text
    const int W3X = 20,  W3Y = 260, W3W = 600, W3H = 190; // Sprites

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;
        double t = game.GetTime();

        game.Clear(COLOR_RGB(18, 20, 28));
        game.DrawText(20, 8, "13_clip_rect: SetClip restricts all drawing to sub-regions", COLOR_LIGHT_GRAY);

        // === Window 1: Shapes ===
        game.SetClip(W1X, W1Y, W1W, W1H);
        game.FillRect(W1X, W1Y, W1W, W1H, COLOR_RGB(26, 36, 52));

        // Large triangle that extends beyond clip
        game.FillTriangle(W1X + W1W / 2, W1Y - 30,
                          W1X + W1W + 60, W1Y + W1H + 20,
                          W1X - 60, W1Y + W1H + 20,
                          COLOR_ARGB(100, 255, 180, 40));

        // Sweeping lines
        for (int i = 0; i < 6; i++) {
            int sweep = ((int)(t * 120.0) + i * 50) % (W1W + 140);
            int x0 = W1X - 70 + sweep;
            game.DrawLine(x0, W1Y - 10, x0 + 100, W1Y + W1H + 20,
                          COLOR_ARGB(180, 120 + i * 22, 220 - i * 26, 255));
        }

        // Bouncing circle that goes partially out of bounds
        int cx = W1X + W1W / 2 + (int)(cos(t * 1.7) * (W1W / 2 + 30));
        int cy = W1Y + W1H / 2 + (int)(sin(t * 2.1) * (W1H / 2 - 10));
        game.FillCircle(cx, cy, 28, COLOR_ARGB(200, 255, 90, 120));

        // Ellipse crossing boundary
        game.DrawEllipse(W1X + W1W / 2, W1Y + W1H / 2,
                         W1W / 2 + 40, W1H / 3, COLOR_CYAN);

        game.ClearClip();
        DrawWindowBorder(game, W1X, W1Y, W1W, W1H, "Shape Clip", COLOR_WHITE);

        // === Window 2: Text ===
        game.SetClip(W2X, W2Y, W2W, W2H);
        game.FillRect(W2X, W2Y, W2W, W2H, COLOR_RGB(44, 30, 22));

        // Scrolling marquee
        int marqueeX = W2X + W2W - ((int)(t * 100.0) % (W2W + 400));
        game.DrawText(marqueeX, W2Y + 30, "This text scrolls and is clipped at the window boundary...", COLOR_GOLD);

        // Oscillating text
        int oscX = W2X - 80 + ((int)(t * 60.0) % (W2W + 160));
        game.DrawText(oscX, W2Y + 60, "DrawText is also clipped!", COLOR_WHITE);

        // Clip info
        game.DrawPrintf(W2X + 8, W2Y + 100, COLOR_LIGHT_GRAY,
                        "GetClip: %d,%d %dx%d",
                        game.GetClipX(), game.GetClipY(),
                        game.GetClipW(), game.GetClipH());
        game.DrawPrintf(W2X + 8, W2Y + 120, COLOR_LIGHT_GRAY,
                        "Time: %.1f s", t);

        // Tall text block that overflows bottom
        for (int i = 0; i < 8; i++) {
            game.DrawPrintf(W2X + 8, W2Y + 146 + i * 14, COLOR_GRAY,
                            "Line %d - clipped at bottom", i);
        }

        game.ClearClip();
        DrawWindowBorder(game, W2X, W2Y, W2W, W2H, "Text Clip", COLOR_WHITE);

        // === Window 3: Sprites ===
        game.SetClip(W3X, W3Y, W3W, W3H);
        game.FillRect(W3X, W3Y, W3W, W3H, COLOR_RGB(20, 30, 20));

        // Draw a grid of stars at various scales, some crossing boundaries
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 10; col++) {
                int sx = W3X - 20 + col * 68 + (int)(sin(t + col * 0.7) * 16);
                int sy = W3Y + 10 + row * 64 + (int)(cos(t + row * 1.1) * 10);
                int sz = 24 + row * 16;
                game.DrawSpriteScaled(star, sx, sy, sz, sz);
            }
        }

        // Large sprite crossing right edge
        int bigX = W3X + W3W - 40 + (int)(sin(t * 0.8) * 60);
        game.DrawSpriteScaled(star, bigX, W3Y + W3H / 2 - 40, 80, 80);

        game.ClearClip();
        DrawWindowBorder(game, W3X, W3Y, W3W, W3H, "Sprite Clip", COLOR_WHITE);

        game.DrawText(20, 460, "ESC to quit | All draws are clipped to their window region", COLOR_DARK_GRAY);

        game.Update();
        game.WaitFrame(60);
    }

    game.FreeSprite(star);
    return 0;
}
