// 11_snake.cpp - Snake Game
//
// Classic Snake: use arrow keys to control the snake, eat food to grow,
// game over if you hit the wall or yourself.
// Learn: DrawGrid, FillCell, IsKeyPressed, game state machine, timed movement
//
// Compile: g++ -o 11_snake.exe 11_snake.cpp -mwindows

#include "../GameLib.h"

#define GRID_ROWS 20
#define GRID_COLS 20
#define CELL_SIZE 22
#define MAX_SNAKE 400

int main()
{
    GameLib game;

    int gridW = GRID_COLS * CELL_SIZE;
    int gridH = GRID_ROWS * CELL_SIZE;
    int winW = gridW + 160;  // leave space for info panel on right
    int winH = gridH + 40;   // leave space for title on top
    game.Open(winW, winH, "11 - Snake", true);

    int gridX = 10, gridY = 30;

    // Snake body (store row/col for each segment in array)
    int snakeR[MAX_SNAKE], snakeC[MAX_SNAKE];
    int snakeLen = 3;

    // Initial position
    snakeR[0] = 10; snakeC[0] = 10;
    snakeR[1] = 10; snakeC[1] = 9;
    snakeR[2] = 10; snakeC[2] = 8;

    // Direction: 0=up 1=down 2=left 3=right
    int dir = 3;
    int nextDir = 3;

    // Food
    int foodR = 5, foodC = 15;

    int score = 0;
    bool gameOver = false;
    bool paused = false;

    // Movement timer
    float moveTimer = 0;
    float moveInterval = 0.15f;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        float dt = game.GetDeltaTime();

        if (!gameOver) {
            // Pause
            if (game.IsKeyPressed(KEY_P))
                paused = !paused;

            if (!paused) {
                // Direction input (cannot reverse directly)
                if (game.IsKeyPressed(KEY_UP)    && dir != 1) nextDir = 0;
                if (game.IsKeyPressed(KEY_DOWN)  && dir != 0) nextDir = 1;
                if (game.IsKeyPressed(KEY_LEFT)  && dir != 3) nextDir = 2;
                if (game.IsKeyPressed(KEY_RIGHT) && dir != 2) nextDir = 3;

                // Timed movement
                moveTimer += dt;
                if (moveTimer >= moveInterval) {
                    moveTimer = 0;
                    dir = nextDir;

                    // Calculate new head position
                    int newR = snakeR[0];
                    int newC = snakeC[0];
                    if (dir == 0) newR--;
                    if (dir == 1) newR++;
                    if (dir == 2) newC--;
                    if (dir == 3) newC++;

                    // Wall collision
                    if (newR < 0 || newR >= GRID_ROWS || newC < 0 || newC >= GRID_COLS) {
                        gameOver = true;
                    } else {
                        // Self collision
                        for (int i = 0; i < snakeLen; i++) {
                            if (snakeR[i] == newR && snakeC[i] == newC) {
                                gameOver = true;
                                break;
                            }
                        }
                    }

                    if (!gameOver) {
                        // Eat food?
                        bool ate = (newR == foodR && newC == foodC);

                        // Move snake body (copy from tail forward)
                        if (!ate) {
                            for (int i = snakeLen - 1; i > 0; i--) {
                                snakeR[i] = snakeR[i - 1];
                                snakeC[i] = snakeC[i - 1];
                            }
                        } else {
                            // Grow: first shift all segments back, make room for head
                            if (snakeLen < MAX_SNAKE) snakeLen++;
                            for (int i = snakeLen - 1; i > 0; i--) {
                                snakeR[i] = snakeR[i - 1];
                                snakeC[i] = snakeC[i - 1];
                            }
                            score += 10;
                            // Speed up
                            if (moveInterval > 0.06f) moveInterval -= 0.003f;

                            // New food (ensure not on snake)
                            bool onSnake;
                            do {
                                foodR = GameLib::Random(0, GRID_ROWS - 1);
                                foodC = GameLib::Random(0, GRID_COLS - 1);
                                onSnake = false;
                                for (int i = 0; i < snakeLen; i++) {
                                    if (snakeR[i] == foodR && snakeC[i] == foodC) {
                                        onSnake = true;
                                        break;
                                    }
                                }
                            } while (onSnake);
                        }
                        snakeR[0] = newR;
                        snakeC[0] = newC;
                    }
                }
            }
        } else {
            if (game.IsKeyPressed(KEY_R)) {
                snakeLen = 3;
                snakeR[0] = 10; snakeC[0] = 10;
                snakeR[1] = 10; snakeC[1] = 9;
                snakeR[2] = 10; snakeC[2] = 8;
                dir = 3; nextDir = 3;
                foodR = 5; foodC = 15;
                score = 0;
                moveInterval = 0.15f;
                gameOver = false;
            }
        }

        // --- Drawing ---
        game.Clear(COLOR_BLACK);

        // Title
        game.DrawTextScale(gridX, 5, "SNAKE", COLOR_GREEN, 2);

        // Grid
        game.DrawGrid(gridX, gridY, GRID_ROWS, GRID_COLS, CELL_SIZE, COLOR_DARK_GRAY);

        // Food (red)
        game.FillCell(gridX, gridY, foodR, foodC, CELL_SIZE, COLOR_RED);

        // Snake body
        for (int i = 0; i < snakeLen; i++) {
            uint32_t c = (i == 0) ? COLOR_GREEN : COLOR_DARK_GREEN;
            game.FillCell(gridX, gridY, snakeR[i], snakeC[i], CELL_SIZE, c);
        }

        // Right info panel
        int infoX = gridX + gridW + 15;
        game.DrawText(infoX, 40, "Score:", COLOR_WHITE);
        game.DrawNumber(infoX, 55, score, COLOR_GOLD);

        game.DrawText(infoX, 85, "Length:", COLOR_WHITE);
        game.DrawNumber(infoX, 100, snakeLen, COLOR_CYAN);

        game.DrawText(infoX, 140, "Controls:", COLOR_GRAY);
        game.DrawText(infoX, 158, "Arrows", COLOR_LIGHT_GRAY);
        game.DrawText(infoX, 172, "P: Pause", COLOR_LIGHT_GRAY);

        if (paused && !gameOver) {
            game.FillRect(gridX + gridW / 2 - 50, gridY + gridH / 2 - 15, 100, 30, COLOR_DARK_GRAY);
            game.DrawText(gridX + gridW / 2 - 30, gridY + gridH / 2 - 7, "PAUSED", COLOR_YELLOW);
        }

        if (gameOver) {
            game.FillRect(gridX + gridW / 2 - 80, gridY + gridH / 2 - 30, 160, 70, COLOR_DARK_GRAY);
            game.DrawRect(gridX + gridW / 2 - 80, gridY + gridH / 2 - 30, 160, 70, COLOR_WHITE);
            game.DrawTextScale(gridX + gridW / 2 - 65, gridY + gridH / 2 - 22, "GAME OVER", COLOR_RED, 1);
            game.DrawPrintf(gridX + gridW / 2 - 50, gridY + gridH / 2 + 2, COLOR_WHITE, "Score: %d", score);
            game.DrawText(gridX + gridW / 2 - 55, gridY + gridH / 2 + 20, "R to restart", COLOR_YELLOW);
        }

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
