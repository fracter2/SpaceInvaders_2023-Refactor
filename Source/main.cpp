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


//------------------------------------------------------------------------------------  // TODO Consider removing, mostly redudant but unintrusive organisation
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{    
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;   // TODO make constexpr
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "SPACE INVADERS");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second      // TODO Remove, redundant comment
    
    Game game = { State::STARTSCREEN };                                                 // TODO remove arg if this is the only relavant instance of it (unused feature -> redudant)
    Resources resources;
    game.resources = resources;                                                         // TODO Move into Game constructor (two step init)
    game.Launch();                                                                      // TODO Consider renaming / clarifying that this is basically an "init()" func
                                                                                        // TODO Consider moving into constructor
    
    //--------------------------------------------------------------------------------------

    InitAudioDevice();                                                                  // TODO Move into "Initialization" group right above´, where it belongs

    auto sound = LoadSound("./hitHurt.ogg");                                            // TODO Implement into update code or remove, both here and as a file (unused)
    



    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update                                                                       // TODO Remove redundant comments here/below
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here                                             // TODO old todo, remove along with this whole comment chunk
        //----------------------------------------------------------------------------------

        //if (IsKeyPressed(KEY_SPACE))
        //{
        //    PlaySound(sound);
        //}

        //if (IsKeyPressed(KEY_BACKSPACE))
        //{
        //    StopSound(sound);
        //}

        game.Update();
      

        // Draw                                                                         // TODO Remove redudant comments
        //----------------------------------------------------------------------------------
        BeginDrawing();                                                                 // TODO Consider moving into the game.Render() func for brevity

        ClearBackground(BLACK);

       

        game.Render();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseAudioDevice();                                                                 // TODO Move into comment group below where it obviously belongs
    
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    std::string filename = "level.txt";                                                 // TODO Remove, unused, TODO remove file as well if unused

    return 0;
}