#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>


enum struct State						// TODO Consider moving into game struct, as it is specifically the game state
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};

enum struct EntityType					// TODO Refactor away, the types themself serve this role
{
	PLAYER,
	ENEMY,
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE
};

struct PlayerData						// TODO Consider moving to a leaderboard.h
{
	std::string name;
	int score;
};

struct Player							// TODO Consider moving to it's own file
{										// TODO Make into class, keep constructor, Render() and Update() public
public:

	float x_pos = 0;
	float speed = 7;					// TODO Make static constexpr
	float player_base_height = 70.0f;	// TODO Make static constexpr // TODO this is used as a y position var. merge with x_pos
	float radius = 50;					// TODO Make static constexpr
	int lives = 3;
	int direction = 0;					// TODO Clarify, consider an enum instead
	int activeTexture = 0;				// TODO Clarify, if this is the texture index used, name it so. Review usage and consider enum
	float timer = 0;					// TODO Clarify

	EntityType type = EntityType::PLAYER;		// TODO What is this for? Refactor away

	void Initialize();					// TODO Refactor into a contructor
	void Render(Texture2D texture);		// TODO Make const
	void Update();
	
};


struct Projectile						// TODO Consider moving to it's own file
{										// TODO Make into class
public: 
	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = {0,0};			// TODO Add a constructor with position param
	int speed = 15;						// TODO Make static constexpr
	bool active = true;					// TODO Remove, shouldn't be needed if inside vector (consider renaming to queueDelete)
	EntityType type = {};				// TODO Remove, shouldn't be needed as long as we dont spawn our projectiles inside ourselves

	// LINE WILL UPDATE WITH POSITION FOR CALCULATIONS
	Vector2 lineStart = { 0, 0 };		// TODO Remove, we already have pos... can make a "length" static constexpr to get start/end points
	Vector2 lineEnd = { 0, 0 };			// TODO Remove

	void Update();

	void Render(Texture2D texture);		// TODO Make const
};

struct Wall 							// TODO Consider moving to it's own file
{										// TODO Make into class
public: 
	Vector2 position;					// TODO Add constructor with pos param
	Rectangle rec;						// TODO Is this used, even though there is a position and radius?
	bool active; 						// TODO Add default // TODO Is this even needed? why not just remove, if it's in a vector?
	Color color;						// TODO Add default and make static constexpr
	int health = 50;
	int radius = 60;					// TODO make constexpr, doesn't change


	void Render(Texture2D texture);		// TODO Make const
	void Update(); 
};

struct Alien							// TODO Consider moving to it's own file
{										// TODO Make into class
public:
	
	Color color = WHITE;				// TODO Make static constexpr
	Vector2 position = {0, 0};			// TODO Add constructor with pos param
	int x = 0;							// TODO Remove, positino should already have this
	int y = 0; 							// TODO Remove, positino should already have this
	float radius = 30;					// TODO Make static constexpr
	bool active = true;					// TODO Remove, shouldn't be needed if in a vector
	bool moveRight = true;				// TODO Rename to clarify it's a variable ("movingRight" or similar) as it sounds like an action (func-like)
	
	EntityType type = EntityType::ENEMY; 

	int speed = 2; 
		 
	void Update(); 
	void Render(Texture2D texture); 
};


struct Star	// TODO Refactor away into Background struct (the only user)
{
	Vector2 initPosition = { 0, 0 };
	Vector2 position = { 0, 0 };		// TODO Irrellavant because it's set to the same for all starts, see Background. Refactor away.
	Color color = GRAY;					// TODO Constexpr or remove
	float size = 0;
	void Update(float starOffset);		// TODO Irrellavant because it's set to the same for all starts, see Background. Refactor away.
	void Render();
};

struct Background	// TODO Consider moving to separate file
{
	std::vector<Star> Stars;

	void Initialize(int starAmount); // TODO Refactor into a constructor
	void Update(float offset);
	void Render();

};

// TODO apply const, constexpr and noexcept where applicable
struct Game
{
	// Gamestate								// TODO Remove redudant comments
	State gameState = {};						// TODO Separate non-gameplay info and states into their own classes

	// Score
	int score;									// TODO Add default 

	// for later, make a file where you can adjust the number of walls (config file)	// TODO Consider, do or remove
	int wallCount = 5;

	//Aliens shooting							// TODO Clarify comment, then if logic already is self-explanatory, remove
	float shootTimer = 0;

	//Aliens stuff? (idk cause liv wrote this)
	Rectangle rec = { 0, 0 ,0 ,0 };				// TODO Rename to clarify

	int formationWidth = 8;						// TODO Set static constexpr for all these
	int formationHeight = 5;
	int alienSpacing = 80;
	int formationX = 100;
	int formationY = 50;

	bool newHighScore = false;					// TODO Consider renaming to "queueNewHighscore" or "newHighscoreTip"
	
	
	void Start();								// TODO Rename to clarify what these do 
	void End();

	void Continue();							// TODO Rename to clarify transition to start screen
	void Launch();								// TODO Refactor away

	void Update();
	void Render();								// TODO Make const

	void SpawnAliens();

	bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineTop, Vector2 lineBottom);	// TODO Rename to clarify that it checks line-circle overlap

	bool CheckNewHighScore();					// TODO Rename to clarify, like "isHighScore()" or "justBeatHighScore()"

	void InsertNewHighScore(std::string name);	// TODO Consider using string_view

	void LoadLeaderboard();						// TODO Consider implementing or removing (unused)
	void SaveLeaderboard();						// TODO Finish implementing this or remove (used but incomplete)


	// Entity Storage and Resources
	Resources resources;						// TODO Consider refactoring into namespace / static vars or into Game class. Does this need to be a separate struct? 

	Player player;

	std::vector<Projectile> Projectiles;

	std::vector<Wall> Walls;

	std::vector<Alien> Aliens;

	std::vector<PlayerData> Leaderboard = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };
	
	Background background;



	Vector2 playerPos;		// TODO Remove, this should be part of the Player class
	Vector2 alienPos;		// TODO Remove, this should be part of the Alien class
	Vector2 cornerPos;		// TODO Remove, used as a local variable
	float offset;			// TODO Remove, used as a local variable



	//TEXTBOX ENTER																			// TODO Make into it's own class, including vars below
	char name[9 + 1] = "\0";      //One extra space required for null terminator char '\0'	// TODO Make into string
	int letterCount = 0;																	// TODO Remove, redundant if name is a string

	Rectangle textBox = { 600, 500, 225, 50 };												
	bool mouseOnText = false;																// TODO Remove, used as a local variable

	int framesCounter = 0;																	// TODO Move into textbox class

};