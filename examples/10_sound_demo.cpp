// 10_sound_demo.cpp - Sound Demo
//
// Demo GameLib sound features: beep, WAV sound effects, MCI background music.
// Note: PlayBeep is blocking, will pause briefly when pressed.
// WAV/Music need corresponding files to exist, otherwise silent fail.
// Learn: PlayBeep, PlayWAV, StopWAV, PlayMusic, StopMusic
//
// Compile: g++ -o 10_sound_demo.exe 10_sound_demo.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "10 - Sound Demo", true);

    bool musicPlaying = false;

    // Key note frequencies (C4 to B4)
    int notes[] = {262, 294, 330, 349, 392, 440, 494, 523};
    const char *noteNames[] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"};

    // Currently active button (for visual feedback)
    int activeNote = -1;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Clear(COLOR_DARK_BLUE);

        // --- Title ---
        game.DrawTextScale(180, 20, "Sound Demo", COLOR_GOLD, 2);

        // === Section 1: Beep Piano ===
        game.DrawText(40, 70, "Piano (Beep) - Press 1~8:", COLOR_WHITE);
        game.DrawText(40, 85, "Warning: Beep is blocking!", COLOR_ORANGE);

        activeNote = -1;
        for (int i = 0; i < 8; i++) {
            int bx = 40 + i * 70;
            int by = 105;
            bool pressed = game.IsKeyPressed(KEY_1 + i);

            if (pressed) activeNote = i;

            // Piano key
            uint32_t keyColor = (activeNote == i) ? COLOR_YELLOW : COLOR_WHITE;
            game.FillRect(bx, by, 55, 100, keyColor);
            game.DrawRect(bx, by, 55, 100, COLOR_BLACK);
            game.DrawText(bx + 20, by + 75, noteNames[i], COLOR_BLACK);

            if (pressed)
                game.PlayBeep(notes[i], 150);
        }

        // === Section 2: WAV Sound Effect ===
        game.DrawText(40, 230, "WAV Sound Effect:", COLOR_WHITE);

        game.FillRect(40, 250, 200, 30, COLOR_GREEN);
        game.DrawText(55, 258, "W - Play WAV", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_W)) {
            // Try to play WAV file from assets folder
            game.PlayWAV("../assets/sound.wav");
        }

        game.FillRect(260, 250, 200, 30, COLOR_RED);
        game.DrawText(290, 258, "S - Stop WAV", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_S))
            game.StopWAV();

        game.DrawText(40, 290, "(needs ../assets/sound.wav)", COLOR_GRAY);

        // === Section 3: Background Music ===
        game.DrawText(40, 330, "Background Music (MCI):", COLOR_WHITE);

        game.FillRect(40, 350, 200, 30, musicPlaying ? COLOR_DARK_GREEN : COLOR_GREEN);
        game.DrawText(55, 358, "M - Play Music", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_M) && !musicPlaying) {
            game.PlayMusic("../assets/music.mp3");
            musicPlaying = true;
        }

        game.FillRect(260, 350, 200, 30, COLOR_RED);
        game.DrawText(275, 358, "N - Stop Music", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_N) && musicPlaying) {
            game.StopMusic();
            musicPlaying = false;
        }

        game.DrawText(40, 390, "(needs ../assets/music.mp3)", COLOR_GRAY);

        // Status
        game.DrawPrintf(40, 420, COLOR_LIGHT_GRAY, "Music: %s", musicPlaying ? "Playing" : "Stopped");

        // Bottom hint
        game.DrawText(40, 460, "ESC to exit", COLOR_DARK_GRAY);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
