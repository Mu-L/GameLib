// 18_tilemap_file.cpp - Tilemap Save/Load Minimal Demo
//
// Build a small tilemap in code, save it to a .glm file,
// then load it back and draw the loaded map.
//
// Learn: CreateSprite, SetSpritePixel, CreateTilemap,
//        SaveTilemap, LoadTilemap, DrawTilemap
//
// Compile: g++ -o 18_tilemap_file.exe 18_tilemap_file.cpp -mwindows

#include "../GameLib.h"

static const int TS = 16;
static const int MAP_COLS = 20;
static const int MAP_ROWS = 15;

enum {
    TILE_SKY = 0,
    TILE_GRASS = 1,
    TILE_BRICK = 2,
    TILE_STONE = 3
};

static void SpriteFill(GameLib &game, int spriteId, int x0, int y0, int w, int h, uint32_t color)
{
    for (int y = y0; y < y0 + h; y++) {
        for (int x = x0; x < x0 + w; x++) {
            game.SetSpritePixel(spriteId, x, y, color);
        }
    }
}

static int MakeTileset(GameLib &game)
{
    int spriteId = game.CreateSprite(4 * TS, TS);
    if (spriteId < 0) return -1;

    int ox = 0;

    ox = TILE_SKY * TS;
    SpriteFill(game, spriteId, ox, 0, TS, TS, COLOR_RGB(135, 206, 235));
    SpriteFill(game, spriteId, ox + 3, 4, 3, 2, COLOR_WHITE);
    SpriteFill(game, spriteId, ox + 10, 7, 4, 2, COLOR_WHITE);

    ox = TILE_GRASS * TS;
    SpriteFill(game, spriteId, ox, 0, TS, TS, COLOR_RGB(110, 78, 48));
    SpriteFill(game, spriteId, ox, 0, TS, 4, COLOR_RGB(60, 180, 75));
    SpriteFill(game, spriteId, ox, 4, TS, 2, COLOR_RGB(95, 155, 60));

    ox = TILE_BRICK * TS;
    SpriteFill(game, spriteId, ox, 0, TS, TS, COLOR_RGB(176, 88, 64));
    for (int y = 3; y < TS; y += 5) {
        SpriteFill(game, spriteId, ox, y, TS, 1, COLOR_RGB(92, 42, 30));
    }
    SpriteFill(game, spriteId, ox + 7, 0, 1, 8, COLOR_RGB(92, 42, 30));
    SpriteFill(game, spriteId, ox + 4, 8, 1, 8, COLOR_RGB(92, 42, 30));
    SpriteFill(game, spriteId, ox + 11, 8, 1, 8, COLOR_RGB(92, 42, 30));

    ox = TILE_STONE * TS;
    SpriteFill(game, spriteId, ox, 0, TS, TS, COLOR_RGB(120, 128, 138));
    SpriteFill(game, spriteId, ox, 0, TS, 1, COLOR_RGB(150, 160, 172));
    SpriteFill(game, spriteId, ox, TS - 1, TS, 1, COLOR_RGB(78, 84, 92));
    SpriteFill(game, spriteId, ox, 0, 1, TS, COLOR_RGB(150, 160, 172));
    SpriteFill(game, spriteId, ox + TS - 1, 0, 1, TS, COLOR_RGB(78, 84, 92));
    SpriteFill(game, spriteId, ox + 4, 4, 3, 3, COLOR_RGB(90, 98, 108));
    SpriteFill(game, spriteId, ox + 10, 8, 4, 3, COLOR_RGB(90, 98, 108));

    return spriteId;
}

static int BuildSampleMap(GameLib &game, int tilesetId)
{
    int mapId = game.CreateTilemap(MAP_COLS, MAP_ROWS, TS, tilesetId);
    if (mapId < 0) return -1;

    game.ClearTilemap(mapId, TILE_SKY);
    game.FillTileRect(mapId, 0, 12, MAP_COLS, 1, TILE_GRASS);
    game.FillTileRect(mapId, 0, 13, MAP_COLS, 2, TILE_STONE);
    game.FillTileRect(mapId, 3, 9, 5, 1, TILE_BRICK);
    game.FillTileRect(mapId, 11, 8, 4, 1, TILE_BRICK);
    game.FillTileRect(mapId, 15, 10, 3, 1, TILE_BRICK);

    for (int step = 0; step < 4; step++) {
        game.SetTile(mapId, 8 + step, 11 - step, TILE_STONE);
    }

    return mapId;
}

int main()
{
    GameLib game;
    const int screenW = 640;
    const int screenH = 480;
    const char *mapFile = "tilemap_demo.glm";

    game.Open(screenW, screenH, "18 - Tilemap Save and Load", true);

    int tilesetId = MakeTileset(game);
    int mapId = -1;
    bool saveOk = false;
    bool loadOk = false;

    if (tilesetId >= 0) {
        int sourceMap = BuildSampleMap(game, tilesetId);
        if (sourceMap >= 0) {
            saveOk = game.SaveTilemap(mapFile, sourceMap);
            if (saveOk) {
                int loadedMap = game.LoadTilemap(mapFile, tilesetId);
                if (loadedMap >= 0) {
                    game.FreeTilemap(sourceMap);
                    mapId = loadedMap;
                    loadOk = true;
                } else {
                    mapId = sourceMap;
                }
            } else {
                mapId = sourceMap;
            }
        }
    }

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Clear(COLOR_RGB(24, 28, 42));
        game.FillRect(0, 0, screenW, 68, COLOR_RGB(12, 16, 26));
        game.DrawText(16, 14, "Create map -> SaveTilemap -> LoadTilemap", COLOR_WHITE);
        game.DrawPrintf(16, 32, COLOR_LIGHT_GRAY, "File: %s", mapFile);
        game.DrawPrintf(16, 48, COLOR_LIGHT_GRAY,
                        "Save: %s   Load: %s   ESC: quit",
                        saveOk ? "OK" : "FAILED",
                        loadOk ? "OK" : "FAILED");

        if (tilesetId < 0) {
            game.DrawText(16, 100, "CreateSprite failed.", COLOR_RED);
        } else if (mapId < 0) {
            game.DrawText(16, 100, "CreateTilemap or LoadTilemap failed.", COLOR_RED);
        } else {
            int drawX = (screenW - MAP_COLS * TS) / 2;
            int drawY = 120;
            game.DrawTilemap(mapId, drawX, drawY);
            game.DrawRect(drawX - 1, drawY - 1, MAP_COLS * TS + 2, MAP_ROWS * TS + 2, COLOR_WHITE);
            game.DrawText(16, 420, "This demo writes a plain-text .glm file to the current working directory.", COLOR_GRAY);
        }

        game.Update();
        game.WaitFrame(60);
    }

    if (mapId >= 0) game.FreeTilemap(mapId);
    if (tilesetId >= 0) game.FreeSprite(tilesetId);
    return 0;
}