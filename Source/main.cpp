/******************************************************************************************* 
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

// This project is a "best practice" refactor of Liv and Eric's Space Invaders remake, which
// itself is based on the source code of "raylib [core] example - Basic window", as seen 
// above.
//
// Refactor by Theodor Rydberg, for the final asignment of the "Modern c++" course 2025-2026


#include "raylib.h"
#include "game.h"

int main(void)
{    
    const int screenWidth = 1920;   // TODO make constexpr
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "SPACE INVADERS");
    SetTargetFPS(60);
    
    Game game = {};
    Resources resources;
    game.resources = resources;                                                         // TODO Move into Game constructor (two step init)
    game.Launch();                                                                      // TODO Consider renaming / clarifying that this is basically an "init()" func
                                                                                        // TODO Consider moving into constructor

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        game.Update();      // TODO Create a scene manager "App" that handles the "scenes" Game, Menu and EndScreen, each having a signal / emitter callback
      
        BeginDrawing();                                                                 // TODO Consider moving into the game.Render() func for brevity
        ClearBackground(BLACK);
        game.Render();
        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}