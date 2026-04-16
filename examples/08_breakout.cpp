// 08_breakout.cpp - Breakout
//
// Classic Breakout: bounce the ball with paddle, destroy all bricks to win.
// Learn: in-depth collision detection, multi-object management, game state
//
// Compile (Win32): g++ -o 08_breakout.exe 08_breakout.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 08_breakout 08_breakout.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

#include <math.h>
#include <stdio.h>

#define BRICK_ROWS 6
#define BRICK_COLS 10
#define BRICK_W 58
#define BRICK_H 18
#define BRICK_GAP 4
#define BRICK_OFFSET_X 12
#define BRICK_OFFSET_Y 50

static const char *ChooseExistingPath(const char *pathA, const char *pathB)
{
    FILE *file = fopen(pathA, "rb");
    if (file != NULL) { fclose(file); return pathA; }
    file = fopen(pathB, "rb");
    if (file != NULL) { fclose(file); return pathB; }
    return pathA;
}

int main()
{
    GameLib game;
    game.Open(640, 480, "08 - Breakout", true);

    const char *launchSfx  = ChooseExistingPath("../assets/sound/jump.wav", "assets/sound/jump.wav");
    const char *bounceSfx  = ChooseExistingPath("../assets/sound/hit.wav", "assets/sound/hit.wav");
    const char *brickRowSfx[BRICK_ROWS] = {
        ChooseExistingPath("../assets/sound/note_do_high.wav", "assets/sound/note_do_high.wav"),
        ChooseExistingPath("../assets/sound/note_si.wav", "assets/sound/note_si.wav"),
        ChooseExistingPath("../assets/sound/note_la.wav", "assets/sound/note_la.wav"),
        ChooseExistingPath("../assets/sound/note_sol.wav", "assets/sound/note_sol.wav"),
        ChooseExistingPath("../assets/sound/note_fa.wav", "assets/sound/note_fa.wav"),
        ChooseExistingPath("../assets/sound/note_mi.wav", "assets/sound/note_mi.wav")
    };
    const char *loseLifeSfx = ChooseExistingPath("../assets/sound/explosion.wav", "assets/sound/explosion.wav");
    const char *restartSfx  = ChooseExistingPath("../assets/sound/click.wav", "assets/sound/click.wav");
    const char *gameOverSfx = ChooseExistingPath("../assets/sound/game_over.wav", "assets/sound/game_over.wav");
    const char *winSfx      = ChooseExistingPath("../assets/sound/victory.wav", "assets/sound/victory.wav");

    bool bricks[BRICK_ROWS][BRICK_COLS];
    uint32_t brickColors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_PURPLE};

    int padW = 80, padH = 12;
    int padX = 280, padY = 450;

    float ballX = 320, ballY = 430;
    float ballVX = 3.0f, ballVY = -4.0f;
    int ballR = 5;

    int score = 0, lives = 3, totalBricks = 0;
    bool started = false, gameOver = false, gameWin = false;

    for (int r = 0; r < BRICK_ROWS; r++)
        for (int c = 0; c < BRICK_COLS; c++)
            bricks[r][c] = true;
    totalBricks = BRICK_ROWS * BRICK_COLS;

    while (!game.IsClosed()) {
        const char *sfxToPlay = NULL;
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        if (!gameOver && !gameWin) {
            if (game.IsKeyDown(KEY_LEFT))  padX -= 6;
            if (game.IsKeyDown(KEY_RIGHT)) padX += 6;
            if (padX < 0) padX = 0;
            if (padX + padW > game.GetWidth()) padX = game.GetWidth() - padW;

            if (!started) {
                ballX = (float)(padX + padW / 2);
                ballY = (float)(padY - ballR - 1);
                if (game.IsKeyPressed(KEY_SPACE)) {
                    started = true;
                    ballVX = 3.0f; ballVY = -4.0f;
                    sfxToPlay = launchSfx;
                }
            } else {
                ballX += ballVX;
                ballY += ballVY;

                if (ballX - ballR < 0) {
                    ballX = (float)ballR; ballVX = -ballVX;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }
                if (ballX + ballR > game.GetWidth()) {
                    ballX = (float)(game.GetWidth() - ballR); ballVX = -ballVX;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }
                if (ballY - ballR < 0) {
                    ballY = (float)ballR; ballVY = -ballVY;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }
                if (ballY + ballR > game.GetHeight()) {
                    lives--;
                    if (lives <= 0) { gameOver = true; sfxToPlay = gameOverSfx; }
                    else { started = false; ballVX = 3.0f; ballVY = -4.0f; sfxToPlay = loseLifeSfx; }
                }

                if (ballVY > 0 &&
                    ballX + ballR > padX && ballX - ballR < padX + padW &&
                    ballY + ballR >= padY && ballY + ballR <= padY + padH + 4) {
                    ballVY = -ballVY;
                    ballY = (float)(padY - ballR);
                    float hitPos = (ballX - padX) / padW;
                    ballVX = (hitPos - 0.5f) * 8.0f;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }

                for (int r = 0; r < BRICK_ROWS; r++) {
                    for (int c = 0; c < BRICK_COLS; c++) {
                        if (!bricks[r][c]) continue;
                        int bx = BRICK_OFFSET_X + c * (BRICK_W + BRICK_GAP);
                        int by = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);

                        if (ballX + ballR > bx && ballX - ballR < bx + BRICK_W &&
                            ballY + ballR > by && ballY - ballR < by + BRICK_H) {
                            bricks[r][c] = false;
                            totalBricks--;
                            score += 10 * (BRICK_ROWS - r);

                            float overlapLeft   = (ballX + ballR) - bx;
                            float overlapRight  = (bx + BRICK_W) - (ballX - ballR);
                            float overlapTop    = (ballY + ballR) - by;
                            float overlapBottom = (by + BRICK_H) - (ballY - ballR);
                            float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
                            float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

                            if (minOverlapX < minOverlapY) ballVX = -ballVX;
                            else ballVY = -ballVY;

                            if (totalBricks <= 0) { gameWin = true; sfxToPlay = winSfx; }
                            else sfxToPlay = brickRowSfx[r];
                            goto done_collision;
                        }
                    }
                }
                done_collision:;
            }
        } else {
            if (game.IsKeyPressed(KEY_R)) {
                for (int r = 0; r < BRICK_ROWS; r++)
                    for (int c = 0; c < BRICK_COLS; c++)
                        bricks[r][c] = true;
                totalBricks = BRICK_ROWS * BRICK_COLS;
                score = 0; lives = 3; padX = 280;
                started = false; gameOver = false; gameWin = false;
                sfxToPlay = restartSfx;
            }
        }

        if (sfxToPlay) game.PlayWAV(sfxToPlay);

        game.Clear(COLOR_BLACK);

        game.DrawPrintf(10, 10, COLOR_WHITE, "Score: %d", score);
        game.DrawPrintf(10, 25, COLOR_GREEN, "Lives: %d", lives);
        game.DrawPrintf(game.GetWidth() - 130, 10, COLOR_GRAY, "Bricks: %d", totalBricks);

        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                if (!bricks[r][c]) continue;
                int bx = BRICK_OFFSET_X + c * (BRICK_W + BRICK_GAP);
                int by = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);
                game.FillRect(bx, by, BRICK_W, BRICK_H, brickColors[r]);
                game.DrawRect(bx, by, BRICK_W, BRICK_H, COLOR_WHITE);
            }
        }

        game.FillRect(padX, padY, padW, padH, COLOR_WHITE);
        game.FillCircle((int)ballX, (int)ballY, ballR, COLOR_WHITE);

        if (!started && !gameOver && !gameWin)
            game.DrawText(240, 420, "SPACE to launch", COLOR_YELLOW);

        if (gameOver) {
            game.FillRect(200, 200, 240, 80, COLOR_DARK_GRAY);
            game.DrawRect(200, 200, 240, 80, COLOR_WHITE);
            game.DrawTextScale(230, 210, "GAME OVER", COLOR_RED, 2);
            game.DrawPrintf(260, 245, COLOR_WHITE, "Score: %d", score);
            game.DrawText(245, 262, "R to restart", COLOR_YELLOW);
        }
        if (gameWin) {
            game.FillRect(200, 200, 240, 80, COLOR_DARK_GRAY);
            game.DrawRect(200, 200, 240, 80, COLOR_WHITE);
            game.DrawTextScale(240, 210, "YOU WIN!", COLOR_GREEN, 2);
            game.DrawPrintf(260, 245, COLOR_WHITE, "Score: %d", score);
            game.DrawText(245, 262, "R to restart", COLOR_YELLOW);
        }

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
