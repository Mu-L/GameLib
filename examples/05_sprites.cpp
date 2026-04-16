// 05_sprites.cpp - Sprites & Animation
//
// Tab key to switch between two modes:
//   Mode A: Sprite basics - create sprite with code, draw, flip, scale, frame extraction
//   Mode B: Sprite animation - character walks with direction-based frame animation
// Learn: CreateSprite, SetSpritePixel, DrawSprite, DrawSpriteEx, DrawSpriteScaled,
//        DrawSpriteFrameScaled, FreeSprite, GetDeltaTime
//
// Compile (Win32): g++ -o 05_sprites.exe 05_sprites.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 05_sprites 05_sprites.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

// ---------- Mode A helpers ----------

// 16x16 small ship sprite, drawn with code
int CreateShipSprite(GameLib &game)
{
    int id = game.CreateSprite(16, 16);
    if (id < 0) return -1;

    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 16; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);

    // Body (cyan)
    for (int y = 4; y < 14; y++)
        for (int x = 6; x < 10; x++)
            game.SetSpritePixel(id, x, y, COLOR_CYAN);

    // Nose (white)
    game.SetSpritePixel(id, 7, 2, COLOR_WHITE);
    game.SetSpritePixel(id, 8, 2, COLOR_WHITE);
    game.SetSpritePixel(id, 7, 3, COLOR_WHITE);
    game.SetSpritePixel(id, 8, 3, COLOR_WHITE);

    // Left wing
    for (int x = 1; x < 6; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }
    // Right wing
    for (int x = 10; x < 15; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }

    // Tail
    game.SetSpritePixel(id, 5, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 10, 13, COLOR_DARK_GRAY);

    // Engine flame
    game.SetSpritePixel(id, 7, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 8, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 7, 15, COLOR_YELLOW);
    game.SetSpritePixel(id, 8, 15, COLOR_YELLOW);

    return id;
}

// 32x8 sprite sheet, 4 frames each 8x8 (pulsing circle animation)
int CreateAnimSheet(GameLib &game)
{
    int id = game.CreateSprite(32, 8);
    if (id < 0) return -1;

    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 32; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);

    uint32_t colors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_WHITE};
    int sizes[] = {1, 2, 3, 2};
    for (int f = 0; f < 4; f++) {
        int cx = f * 8 + 4;
        int cy = 4;
        int s = sizes[f];
        for (int dy = -s; dy <= s; dy++)
            for (int dx = -s; dx <= s; dx++)
                if (dx*dx + dy*dy <= s*s)
                    game.SetSpritePixel(id, cx + dx, cy + dy, colors[f]);
    }
    return id;
}

// ---------- Mode B helpers ----------

// 36x64 character sprite sheet: 4 directions x 3 frames, each 12x16
int CreateCharSheet(GameLib &game)
{
    int fw = 12, fh = 16;
    int cols = 3, rows = 4;
    int id = game.CreateSprite(fw * cols, fh * rows);
    if (id < 0) return -1;

    for (int y = 0; y < fh * rows; y++)
        for (int x = 0; x < fw * cols; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);

    uint32_t skin  = COLOR_RGB(255, 200, 150);
    uint32_t hair  = COLOR_BROWN;
    uint32_t shirt = COLOR_BLUE;
    uint32_t pants = COLOR_DARK_BLUE;
    uint32_t shoe  = COLOR_DARK_GRAY;

    for (int dir = 0; dir < 4; dir++) {
        for (int f = 0; f < 3; f++) {
            int ox = f * fw;
            int oy = dir * fh;

            // Head (4x4)
            for (int dy = 0; dy < 4; dy++)
                for (int dx = 0; dx < 4; dx++)
                    game.SetSpritePixel(id, ox + 4 + dx, oy + dy, skin);
            // Hair
            for (int dx = 0; dx < 4; dx++)
                game.SetSpritePixel(id, ox + 4 + dx, oy, hair);
            // Body (6x5)
            for (int dy = 4; dy < 9; dy++)
                for (int dx = 0; dx < 6; dx++)
                    game.SetSpritePixel(id, ox + 3 + dx, oy + dy, shirt);
            // Pants (6x3)
            for (int dy = 9; dy < 12; dy++)
                for (int dx = 0; dx < 6; dx++)
                    game.SetSpritePixel(id, ox + 3 + dx, oy + dy, pants);
            // Feet
            int leftFootX, rightFootX;
            if (f == 0) { leftFootX = 3; rightFootX = 7; }
            else if (f == 1) { leftFootX = 2; rightFootX = 8; }
            else { leftFootX = 4; rightFootX = 6; }
            for (int dy = 12; dy < 14; dy++) {
                game.SetSpritePixel(id, ox + leftFootX, oy + dy, shoe);
                game.SetSpritePixel(id, ox + leftFootX + 1, oy + dy, shoe);
                game.SetSpritePixel(id, ox + rightFootX, oy + dy, shoe);
                game.SetSpritePixel(id, ox + rightFootX + 1, oy + dy, shoe);
            }
            // Eyes
            if (dir == 0) {
                game.SetSpritePixel(id, ox + 5, oy + 2, COLOR_BLACK);
                game.SetSpritePixel(id, ox + 7, oy + 2, COLOR_BLACK);
            } else if (dir == 1) {
                game.SetSpritePixel(id, ox + 4, oy + 2, COLOR_BLACK);
            } else if (dir == 2) {
                game.SetSpritePixel(id, ox + 7, oy + 2, COLOR_BLACK);
            } else {
                for (int dx = 0; dx < 4; dx++)
                    game.SetSpritePixel(id, ox + 4 + dx, oy + 1, hair);
            }
        }
    }
    return id;
}

int main()
{
    GameLib game;
    game.Open(640, 480, "05 - Sprites & Animation", true);

    int ship      = CreateShipSprite(game);
    int animSheet = CreateAnimSheet(game);
    int charSheet = CreateCharSheet(game);

    // Mode A state
    int shipX = 300, shipY = 350;
    int animFrame = 0, animTimer = 0;

    // Mode B state
    int fw = 12, fh = 16, charScale = 3;
    double px = 300.0, py = 220.0, charSpeed = 100.0;
    int charDir = 0, charFrame = 0;
    double charAnimTimer = 0.0;
    bool charMoving = false;

    bool modeB = false;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;
        if (game.IsKeyPressed(KEY_TAB)) modeB = !modeB;

        double dt = game.GetDeltaTime();

        if (!modeB) {
            // --- Mode A: Sprite Basics ---

            // Move ship
            if (game.IsKeyDown(KEY_LEFT))  shipX -= 3;
            if (game.IsKeyDown(KEY_RIGHT)) shipX += 3;
            if (game.IsKeyDown(KEY_UP))    shipY -= 3;
            if (game.IsKeyDown(KEY_DOWN))  shipY += 3;

            // Animate sheet frames
            animTimer++;
            if (animTimer >= 10) {
                animTimer = 0;
                animFrame = (animFrame + 1) % 4;
            }

            game.Clear(COLOR_BLACK);

            game.DrawText(20, 20, "DrawSprite (normal):", COLOR_WHITE);
            game.DrawSprite(ship, 20, 40);

            game.DrawText(20, 70, "DrawSpriteEx (flipped):", COLOR_WHITE);
            game.DrawSpriteEx(ship, 20, 90, SPRITE_FLIP_H);
            game.DrawText(50, 95, "H", COLOR_GRAY);
            game.DrawSpriteEx(ship, 80, 90, SPRITE_FLIP_V);
            game.DrawText(110, 95, "V", COLOR_GRAY);
            game.DrawSpriteEx(ship, 140, 90, SPRITE_FLIP_H | SPRITE_FLIP_V);
            game.DrawText(170, 95, "H+V", COLOR_GRAY);

            game.DrawText(20, 130, "DrawSpriteFrameScaled (sprite sheet):", COLOR_WHITE);
            game.DrawSpriteScaled(animSheet, 20, 150, 64, 16);
            game.DrawRect(20, 150, 64, 16, COLOR_GRAY);
            game.DrawText(100, 148, "<-- full sheet", COLOR_GRAY);
            game.DrawText(20, 172, "Current frame:", COLOR_GRAY);
            game.DrawSpriteFrameScaled(animSheet, 130, 162, 8, 8, animFrame, 32, 32);
            game.DrawSpriteFrameScaled(animSheet, 170, 162, 8, 8, animFrame, 32, 32, SPRITE_FLIP_H);

            game.DrawText(20, 200, "Move with arrow keys:", COLOR_WHITE);
            game.DrawSprite(ship, shipX, shipY);

            game.DrawText(400, 20, "DrawSpriteScaled (4x):", COLOR_WHITE);
            game.DrawSpriteScaled(ship, 400, 40, 64, 64);
            game.DrawRect(400, 40, 64, 64, COLOR_GRAY);

            game.DrawText(10, 460, "[TAB] Switch to Animation mode | Arrows: move ship", COLOR_DARK_GRAY);

        } else {
            // --- Mode B: Sprite Animation ---

            charMoving = false;
            if (game.IsKeyDown(KEY_DOWN)  || game.IsKeyDown(KEY_S)) { py += charSpeed * dt; charDir = 0; charMoving = true; }
            if (game.IsKeyDown(KEY_LEFT)  || game.IsKeyDown(KEY_A)) { px -= charSpeed * dt; charDir = 1; charMoving = true; }
            if (game.IsKeyDown(KEY_RIGHT) || game.IsKeyDown(KEY_D)) { px += charSpeed * dt; charDir = 2; charMoving = true; }
            if (game.IsKeyDown(KEY_UP)    || game.IsKeyDown(KEY_W)) { py -= charSpeed * dt; charDir = 3; charMoving = true; }

            if (px < 0) px = 0;
            if (py < 0) py = 0;
            if (px > game.GetWidth() - fw * charScale) px = (double)(game.GetWidth() - fw * charScale);
            if (py > game.GetHeight() - fh * charScale) py = (double)(game.GetHeight() - fh * charScale);

            if (charMoving) {
                charAnimTimer += dt;
                if (charAnimTimer >= 0.15) {
                    charAnimTimer = 0.0;
                    charFrame = (charFrame + 1) % 3;
                }
            } else {
                charFrame = 0;
                charAnimTimer = 0.0;
            }

            game.Clear(COLOR_DARK_GREEN);

            // Ground decoration
            for (int i = 0; i < 30; i++) {
                int gx = (i * 97 + 13) % game.GetWidth();
                int gy = (i * 173 + 47) % game.GetHeight();
                game.FillRect(gx, gy, 3, 3, COLOR_GREEN);
            }

            // Character
            int frameIndex = charDir * 3 + charFrame;
            game.DrawSpriteFrameScaled(charSheet, (int)px, (int)py,
                                       fw, fh, frameIndex,
                                       fw * charScale, fh * charScale);

            // Sheet preview (top right)
            game.DrawText(470, 10, "Sprite Sheet:", COLOR_WHITE);
            int pvScale = 2;
            int pvX = 470, pvY = 25;
            game.DrawSpriteScaled(charSheet, pvX, pvY, fw * 3 * pvScale, fh * 4 * pvScale);
            game.DrawRect(pvX, pvY, fw * 3 * pvScale, fh * 4 * pvScale, COLOR_GRAY);
            game.DrawRect(pvX + charFrame * fw * pvScale, pvY + charDir * fh * pvScale,
                          fw * pvScale, fh * pvScale, COLOR_YELLOW);

            const char *dirNames[] = {"Down", "Left", "Right", "Up"};
            game.DrawText(10, 10, "WASD / Arrow keys to move", COLOR_WHITE);
            game.DrawPrintf(10, 25, COLOR_GRAY, "Dir: %s  Frame: %d", dirNames[charDir], charFrame);
            game.DrawText(10, 460, "[TAB] Switch to Sprite Basics mode", COLOR_DARK_GRAY);
        }

        game.Update();
        game.WaitFrame(60);
    }

    game.FreeSprite(ship);
    game.FreeSprite(animSheet);
    game.FreeSprite(charSheet);
    return 0;
}
