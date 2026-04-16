// 12_sprite_transform.cpp - Sprite Scaling & Rotation
//
// Tab key to switch between two modes:
//   Mode A: Sprite Scaling  - DrawSpriteScaled, DrawSpriteFrameScaled at various sizes
//   Mode B: Sprite Rotation - DrawSpriteRotated, DrawSpriteFrameRotated with animation
//
// Controls:
//   Mode A: Wheel/Q/E change scale, A/D change frame, F flip, R reset
//   Mode B: Wheel/Q/E change angular speed, A/D change frame, F flip, SPACE pause, R reset
//   ESC: quit
//
// Learn: DrawSpriteScaled, DrawSpriteFrameScaled, DrawSpriteRotated,
//        DrawSpriteFrameRotated, SetSpriteColorKey, SPRITE_COLORKEY
//
// Compile (Win32): g++ -o 12_sprite_transform.exe 12_sprite_transform.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 12_sprite_transform 12_sprite_transform.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

static int ClampInt(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void WrapAngle(double *a)
{
    while (*a >= 360.0) *a -= 360.0;
    while (*a < 0.0) *a += 360.0;
}

static void DrawPanel(GameLib &game, int x, int y, int w, int h, const char *title)
{
    game.FillRect(x, y, w, h, COLOR_RGB(28, 34, 50));
    game.DrawRect(x, y, w, h, COLOR_RGB(84, 94, 120));
    game.FillRect(x + 1, y + 1, w - 2, 22, COLOR_RGB(38, 48, 72));
    game.DrawText(x + 8, y + 7, title, COLOR_WHITE);
}

static void DrawCheckerboard(GameLib &game, int x, int y, int w, int h, int cell)
{
    uint32_t c0 = COLOR_RGB(48, 54, 70);
    uint32_t c1 = COLOR_RGB(62, 70, 90);
    for (int py = y; py < y + h; py += cell) {
        for (int px = x; px < x + w; px += cell) {
            int cw = cell, ch = cell;
            if (px + cw > x + w) cw = x + w - px;
            if (py + ch > y + h) ch = y + h - py;
            game.FillRect(px, py, cw, ch,
                          (((px - x) / cell + (py - y) / cell) & 1) ? c0 : c1);
        }
    }
}

static void DrawCrosshair(GameLib &game, int cx, int cy, int size, uint32_t color)
{
    game.DrawLine(cx - size, cy, cx + size, cy, color);
    game.DrawLine(cx, cy - size, cx, cy + size, color);
}

// 17x17 ship sprite (odd size for rotation center)
static int CreateShipSprite(GameLib &game)
{
    int id = game.CreateSprite(17, 17);
    if (id < 0) return -1;

    for (int y = 0; y < 17; y++)
        for (int x = 0; x < 17; x++)
            game.SetSpritePixel(id, x, y, COLORKEY_DEFAULT);

    // Nose
    for (int y = 1; y <= 6; y++) {
        int span = y - 1;
        for (int x = 8 - span; x <= 8 + span; x++)
            game.SetSpritePixel(id, x, y, COLOR_WHITE);
    }
    // Body
    for (int y = 6; y <= 12; y++)
        for (int x = 6; x <= 10; x++)
            game.SetSpritePixel(id, x, y, COLOR_CYAN);
    // Wings
    for (int x = 2; x <= 5; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }
    for (int x = 11; x <= 14; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }
    // Tail
    game.SetSpritePixel(id, 6, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 7, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 9, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 10, 13, COLOR_DARK_GRAY);
    // Flame
    game.SetSpritePixel(id, 7, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 8, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 9, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 7, 15, COLOR_YELLOW);
    game.SetSpritePixel(id, 8, 16, COLOR_YELLOW);
    game.SetSpritePixel(id, 9, 15, COLOR_YELLOW);

    game.SetSpriteColorKey(id, COLORKEY_DEFAULT);
    return id;
}

// 64x16 pulse sheet: 4 frames, each 16x16
static int CreatePulseSheet(GameLib &game)
{
    int id = game.CreateSprite(64, 16);
    if (id < 0) return -1;

    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 64; x++)
            game.SetSpritePixel(id, x, y, COLORKEY_DEFAULT);

    uint32_t colors[4] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_WHITE};
    int radii[4] = {2, 3, 4, 3};

    for (int f = 0; f < 4; f++) {
        int cx = f * 16 + 8, cy = 8, r = radii[f];
        for (int dy = -r; dy <= r; dy++)
            for (int dx = -r; dx <= r; dx++) {
                int ax = dx < 0 ? -dx : dx;
                int ay = dy < 0 ? -dy : dy;
                if (ax + ay <= r)
                    game.SetSpritePixel(id, cx + dx, cy + dy, colors[f]);
            }
        game.SetSpritePixel(id, cx, cy, COLOR_WHITE);
        game.SetSpritePixel(id, cx - 1, cy, COLOR_WHITE);
        game.SetSpritePixel(id, cx + 1, cy, COLOR_WHITE);
        game.SetSpritePixel(id, cx, cy - 1, COLOR_WHITE);
        game.SetSpritePixel(id, cx, cy + 1, COLOR_WHITE);
    }

    game.SetSpriteColorKey(id, COLORKEY_DEFAULT);
    return id;
}

int main()
{
    GameLib game;
    const int SW = 860, SH = 580;
    game.Open(SW, SH, "12 - Sprite Transform", true);

    int ship  = CreateShipSprite(game);
    int pulse = CreatePulseSheet(game);

    // Shared state
    int frame = 0;
    bool flip = false;

    // Mode A (scaling) state
    int scale = 4;

    // Mode B (rotation) state
    int angleSpeed = 3;
    double angle = 0.0;
    bool spinning = true;

    bool modeB = false;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;
        if (game.IsKeyPressed(KEY_TAB)) modeB = !modeB;

        // Shared controls
        if (game.IsKeyPressed(KEY_A) || game.IsKeyPressed(KEY_LEFT))
            frame = (frame + 3) % 4;
        if (game.IsKeyPressed(KEY_D) || game.IsKeyPressed(KEY_RIGHT))
            frame = (frame + 1) % 4;
        if (game.IsKeyPressed(KEY_F)) flip = !flip;

        int wheel = game.GetMouseWheelDelta();
        int flags = SPRITE_COLORKEY | (flip ? SPRITE_FLIP_H : 0);
        const char *flipText = flip ? "On" : "Off";

        if (!modeB) {
            // --- Mode A: Scaling ---
            if (wheel != 0) scale += wheel;
            if (game.IsKeyPressed(KEY_Q)) scale--;
            if (game.IsKeyPressed(KEY_E)) scale++;
            scale = ClampInt(scale, 1, 6);

            if (game.IsKeyPressed(KEY_R)) { scale = 4; frame = 0; flip = false; }

            game.Clear(COLOR_RGB(18, 22, 36));
            game.FillRect(0, 0, SW, 56, COLOR_RGB(10, 14, 24));
            game.DrawText(20, 8, "MODE A: SCALING", COLOR_CYAN);
            game.DrawText(20, 24, "Wheel/Q/E scale  A/D frame  F flip  R reset  TAB switch  ESC quit", COLOR_WHITE);
            game.DrawPrintf(20, 40, COLOR_LIGHT_GRAY,
                            "Scale: %dx   Frame: %d   Flip: %s", scale, frame, flipText);

            // Source panel
            DrawPanel(game, 20, 66, 200, 494, "Source Sprites");
            game.DrawText(36, 98, "Ship (17x17)", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 36, 116, 168, 100, 8);
            game.DrawRect(36, 116, 168, 100, COLOR_RGB(98, 110, 138));
            game.DrawSpriteEx(ship, 36 + (168 - 17) / 2, 116 + (100 - 17) / 2, SPRITE_COLORKEY);

            game.DrawText(36, 236, "Pulse sheet (64x16)", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 36, 256, 168, 80, 8);
            game.DrawRect(36, 256, 168, 80, COLOR_RGB(98, 110, 138));
            game.DrawSpriteScaled(pulse, 52, 276, 128, 32, SPRITE_COLORKEY);
            game.DrawRect(52 + frame * 32, 276, 32, 32, COLOR_GOLD);
            game.DrawText(36, 346, "4 frames, each 16x16", COLOR_GRAY);

            // Scaling panel
            DrawPanel(game, 240, 66, 600, 230, "DrawSpriteScaled");
            game.DrawText(256, 98, "Scaled ship", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 256, 116, 280, 160, 10);
            game.DrawRect(256, 116, 280, 160, COLOR_RGB(98, 110, 138));
            game.DrawSpriteScaled(ship,
                                  256 + (280 - 17 * scale) / 2,
                                  116 + (160 - 17 * scale) / 2,
                                  17 * scale, 17 * scale, flags);
            game.DrawPrintf(256, 282, COLOR_WHITE, "17x17 -> %dx%d", 17 * scale, 17 * scale);

            game.DrawText(560, 98, "Wide stretch", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 560, 116, 120, 72, 10);
            game.DrawRect(560, 116, 120, 72, COLOR_RGB(98, 110, 138));
            game.DrawSpriteScaled(ship, 560 + (120 - 96) / 2, 116 + (72 - 48) / 2,
                                  96, 48, flags);
            game.DrawText(560, 196, "96x48", COLOR_GRAY);

            game.DrawText(700, 98, "Tall stretch", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 700, 116, 120, 160, 10);
            game.DrawRect(700, 116, 120, 160, COLOR_RGB(98, 110, 138));
            game.DrawSpriteScaled(ship, 700 + (120 - 48) / 2, 116 + (160 - 120) / 2,
                                  48, 120, flags);
            game.DrawText(700, 282, "48x120", COLOR_GRAY);

            // Frame scaling panel
            DrawPanel(game, 240, 310, 600, 250, "DrawSpriteFrameScaled");
            game.DrawText(256, 342, "Sheet preview", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 256, 360, 180, 80, 8);
            game.DrawRect(256, 360, 180, 80, COLOR_RGB(98, 110, 138));
            game.DrawSpriteScaled(pulse, 264, 380, 160, 32, SPRITE_COLORKEY);
            game.DrawRect(264 + frame * 40, 380, 40, 32, COLOR_GOLD);
            game.DrawText(256, 448, "A/D changes highlight", COLOR_GRAY);

            game.DrawText(460, 342, "Current frame scaled", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 460, 360, 360, 180, 10);
            game.DrawRect(460, 360, 360, 180, COLOR_RGB(98, 110, 138));
            game.DrawSpriteFrameScaled(pulse,
                                       460 + (360 - 16 * scale) / 2,
                                       360 + (180 - 16 * scale) / 2,
                                       16, 16, frame,
                                       16 * scale, 16 * scale, flags);
            game.DrawPrintf(460, 546, COLOR_WHITE, "16x16 -> %dx%d", 16 * scale, 16 * scale);

        } else {
            // --- Mode B: Rotation ---
            if (wheel > 0) angleSpeed++;
            if (wheel < 0) angleSpeed--;
            if (game.IsKeyPressed(KEY_Q)) angleSpeed--;
            if (game.IsKeyPressed(KEY_E)) angleSpeed++;
            angleSpeed = ClampInt(angleSpeed, -12, 12);
            if (game.IsKeyPressed(KEY_SPACE)) spinning = !spinning;
            if (game.IsKeyPressed(KEY_R)) {
                angle = 0; angleSpeed = 3; frame = 0; flip = false; spinning = true;
            }

            if (spinning) { angle += (double)angleSpeed; WrapAngle(&angle); }

            const char *spinText = spinning ? "Running" : "Paused";

            game.Clear(COLOR_RGB(18, 22, 36));
            game.FillRect(0, 0, SW, 56, COLOR_RGB(10, 14, 24));
            game.DrawText(20, 8, "MODE B: ROTATION", COLOR_CYAN);
            game.DrawText(20, 24, "Wheel/Q/E speed  A/D frame  F flip  SPACE pause  R reset  TAB switch  ESC quit", COLOR_WHITE);
            game.DrawPrintf(20, 40, COLOR_LIGHT_GRAY,
                            "Angle: %.1f   Speed: %d   Frame: %d   Flip: %s   %s",
                            angle, angleSpeed, frame, flipText, spinText);

            // Source panel
            DrawPanel(game, 20, 66, 200, 494, "Source Sprites");
            game.DrawText(36, 98, "Ship (17x17)", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 36, 116, 168, 100, 8);
            game.DrawRect(36, 116, 168, 100, COLOR_RGB(98, 110, 138));
            game.DrawSpriteEx(ship, 36 + (168 - 17) / 2, 116 + (100 - 17) / 2, SPRITE_COLORKEY);

            game.DrawText(36, 236, "Pulse sheet (64x16)", COLOR_LIGHT_GRAY);
            DrawCheckerboard(game, 36, 256, 168, 80, 8);
            game.DrawRect(36, 256, 168, 80, COLOR_RGB(98, 110, 138));
            game.DrawSpriteScaled(pulse, 52, 276, 128, 32, SPRITE_COLORKEY);
            game.DrawRect(52 + frame * 32, 276, 32, 32, COLOR_GOLD);
            game.DrawText(36, 346, "4 frames, each 16x16", COLOR_GRAY);

            // Rotation panel
            DrawPanel(game, 240, 66, 600, 230, "DrawSpriteRotated");
            // Main rotating ship
            DrawCheckerboard(game, 256, 96, 200, 180, 10);
            game.DrawRect(256, 96, 200, 180, COLOR_RGB(98, 110, 138));
            DrawCrosshair(game, 356, 186, 14, COLOR_RGB(120, 138, 168));
            game.DrawSpriteRotated(ship, 356, 186, angle, flags);
            game.DrawText(256, 282, "Animated", COLOR_LIGHT_GRAY);

            // Static angle examples
            DrawCheckerboard(game, 480, 96, 80, 80, 10);
            game.DrawRect(480, 96, 80, 80, COLOR_RGB(98, 110, 138));
            DrawCrosshair(game, 520, 136, 10, COLOR_RGB(120, 138, 168));
            game.DrawSpriteRotated(ship, 520, 136, 0.0, flags);
            game.DrawText(480, 178, "0 deg", COLOR_GRAY);

            DrawCheckerboard(game, 580, 96, 80, 80, 10);
            game.DrawRect(580, 96, 80, 80, COLOR_RGB(98, 110, 138));
            DrawCrosshair(game, 620, 136, 10, COLOR_RGB(120, 138, 168));
            game.DrawSpriteRotated(ship, 620, 136, 45.0, flags);
            game.DrawText(580, 178, "45 deg", COLOR_GRAY);

            DrawCheckerboard(game, 700, 96, 80, 80, 10);
            game.DrawRect(700, 96, 80, 80, COLOR_RGB(98, 110, 138));
            DrawCrosshair(game, 740, 136, 10, COLOR_RGB(120, 138, 168));
            game.DrawSpriteRotated(ship, 740, 136, 90.0, flags);
            game.DrawText(700, 178, "90 deg", COLOR_GRAY);

            DrawCheckerboard(game, 530, 198, 80, 80, 10);
            game.DrawRect(530, 198, 80, 80, COLOR_RGB(98, 110, 138));
            DrawCrosshair(game, 570, 238, 10, COLOR_RGB(120, 138, 168));
            game.DrawSpriteRotated(ship, 570, 238, 135.0, flags);
            game.DrawText(530, 282, "135 deg", COLOR_GRAY);

            DrawCheckerboard(game, 650, 198, 80, 80, 10);
            game.DrawRect(650, 198, 80, 80, COLOR_RGB(98, 110, 138));
            DrawCrosshair(game, 690, 238, 10, COLOR_RGB(120, 138, 168));
            game.DrawSpriteRotated(ship, 690, 238, 180.0, flags);
            game.DrawText(650, 282, "180 deg", COLOR_GRAY);

            // Frame rotation panel
            DrawPanel(game, 240, 310, 600, 250, "DrawSpriteFrameRotated");
            // Main rotating frame
            DrawCheckerboard(game, 256, 340, 180, 160, 10);
            game.DrawRect(256, 340, 180, 160, COLOR_RGB(98, 110, 138));
            DrawCrosshair(game, 346, 420, 12, COLOR_RGB(120, 138, 168));
            game.DrawSpriteFrameRotated(pulse, 346, 420, 16, 16, frame, angle, flags);
            game.DrawText(256, 506, "Current pulse", COLOR_LIGHT_GRAY);

            // Per-frame rotation
            for (int i = 0; i < 4; i++) {
                int bx = 460 + i * 100, by = 340;
                DrawCheckerboard(game, bx, by, 80, 72, 10);
                game.DrawRect(bx, by, 80, 72, COLOR_RGB(98, 110, 138));
                DrawCrosshair(game, bx + 40, by + 36, 10, COLOR_RGB(120, 138, 168));
                game.DrawSpriteFrameRotated(pulse, bx + 40, by + 36,
                                            16, 16, i, angle + i * 30.0, flags);
                game.DrawPrintf(bx, by + 76, COLOR_GRAY, "Frame %d", i);
            }

            // Highlight current frame
            int hlx = 460 + frame * 100;
            game.DrawRect(hlx, 340, 80, 72, COLOR_GOLD);

            game.DrawText(460, 448, "A/D changes selected frame", COLOR_LIGHT_GRAY);
            game.DrawText(460, 468, "Each frame offset by 30 degrees", COLOR_GRAY);
        }

        game.Update();
        game.WaitFrame(60);
    }

    game.FreeSprite(ship);
    game.FreeSprite(pulse);
    return 0;
}
