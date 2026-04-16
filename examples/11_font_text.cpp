// 11_font_text.cpp - Font Text and UI Demo
//
// Demonstrates DrawTextFont, DrawPrintfFont, text measurement,
// ShowMouse, and ShowMessage.
// Learn: DrawTextFont, DrawPrintfFont, GetTextWidthFont, ShowMouse, ShowMessage
//
// Compile (Win32): g++ -o 11_font_text.exe 11_font_text.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 11_font_text 11_font_text.cpp -lSDL2 -lSDL2_ttf

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#define FONT_MONO    "Consolas"
#define FONT_CJK     "MS Gothic"
#else
#include "../GameLib.SDL.h"
#define FONT_MONO    "monospace"
#define FONT_CJK     "sans-serif"
#endif

int main()
{
    GameLib game;
    game.Open(720, 520, "11 - Font Text and UI", true);
    game.ShowFps(true);

    bool mouseVisible = true;
    int lastMessageResult = MESSAGEBOX_RESULT_OK;

    while (!game.IsClosed()) {
        double timeSec = game.GetTime();
        int score = (int)(timeSec * 123.0);
        int titleW = game.GetTextWidthFont("DrawTextFont + DrawPrintfFont", 30);
        const char *resultText = "OK";

        if (lastMessageResult == MESSAGEBOX_RESULT_YES) resultText = "YES";
        else if (lastMessageResult == MESSAGEBOX_RESULT_NO) resultText = "NO";

        if (game.IsKeyPressed(KEY_H)) {
            mouseVisible = !mouseVisible;
            game.ShowMouse(mouseVisible);
        }
        if (game.IsKeyPressed(KEY_M)) {
            lastMessageResult = game.ShowMessage(
                "GameLib now has DrawPrintfFont, ShowMouse and ShowMessage.",
                "New UI APIs",
                MESSAGEBOX_OK);
        }
        if (game.IsKeyPressed(KEY_Y)) {
            lastMessageResult = game.ShowMessage(
                "Show the mouse cursor?",
                "ShowMessage YES/NO",
                MESSAGEBOX_YESNO);
            mouseVisible = (lastMessageResult != MESSAGEBOX_RESULT_NO);
            game.ShowMouse(mouseVisible);
        }
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Clear(COLOR_RGB(18, 24, 34));

        game.FillRect(18, 18, 684, 70, COLOR_ARGB(215, 34, 42, 56));
        game.DrawRect(18, 18, 684, 70, COLOR_LIGHT_GRAY);
        game.DrawTextFont((game.GetWidth() - titleW) / 2, 32,
                          "DrawTextFont + DrawPrintfFont", COLOR_WHITE, 30);
        game.DrawText(30, 74, "Built-in text below is ASCII only. Font text can use Unicode.", COLOR_LIGHT_GRAY);

        game.FillRect(18, 104, 420, 214, COLOR_ARGB(205, 26, 34, 46));
        game.DrawRect(18, 104, 420, 214, COLOR_LIGHT_GRAY);
        game.DrawText(28, 114, "Scalable font text", COLOR_WHITE);
        game.DrawTextFont(28, 142, "Scalable font - supports Unicode", COLOR_YELLOW, 24);
        game.DrawTextFont(28, 178, "CJK font rendering test", COLOR_CYAN, FONT_CJK, 22);
        game.DrawTextFont(28, 214, "Different sizes: 18 / 24 / 32", COLOR_GREEN, 18);
        game.DrawTextFont(28, 240, "Different sizes: 18 / 24 / 32", COLOR_GREEN, 24);
        game.DrawTextFont(28, 274, "Different sizes: 18 / 24 / 32", COLOR_GREEN, 32);

        game.FillRect(456, 104, 246, 214, COLOR_ARGB(205, 34, 42, 56));
        game.DrawRect(456, 104, 246, 214, COLOR_LIGHT_GRAY);
        game.DrawText(466, 114, "DrawPrintfFont", COLOR_WHITE);
        game.DrawPrintfFont(470, 146, COLOR_GOLD, 24, "Score: %05d", score);
        game.DrawPrintfFont(470, 180, COLOR_SKY_BLUE, FONT_MONO, 18,
                            "Time: %5.1f s", timeSec);
        game.DrawPrintfFont(470, 206, COLOR_WHITE, FONT_MONO, 18,
                            "FPS: %5.1f", game.GetFPS());
        game.DrawPrintfFont(470, 232, COLOR_PINK, FONT_MONO, 18,
                            "Mouse: %3d, %3d", game.GetMouseX(), game.GetMouseY());
        game.DrawPrintfFont(470, 258, COLOR_LIGHT_GRAY, FONT_MONO, 18,
                            "Cursor: %s", mouseVisible ? "visible" : "hidden");
        game.DrawPrintfFont(470, 284, COLOR_LIGHT_GRAY, FONT_MONO, 18,
                            "Last dialog: %s", resultText);

        game.FillRect(18, 338, 684, 136, COLOR_ARGB(205, 28, 36, 48));
        game.DrawRect(18, 338, 684, 136, COLOR_LIGHT_GRAY);
        game.DrawText(28, 348, "Interactive UI helpers", COLOR_WHITE);
        game.DrawTextFont(28, 376, "H: Toggle mouse  M: Show OK dialog", COLOR_CYAN, 22);
        game.DrawTextFont(28, 408, "Y: Show YES/NO dialog and sync mouse state", COLOR_CYAN, 22);
        game.DrawTextFont(28, 440, "ESC: Quit", COLOR_GRAY, 18);

        game.Update();
        game.WaitFrame(60);
    }

    return 0;
}
