#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>


enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};

enum struct EntityType
{
	PLAYER,
	ENEMY,
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE
};

struct PlayerData
{
	std::string name;
	int score;
};

struct Player
{
public:									// TODO Consider if any properties should be private, and remove this public: (redundant)

	float x_pos = 0;
	float speed = 7;					// TODO Set static constexpr
	float player_base_height = 70.0f;	// TODO Set static constexpr
	float radius = 50;					// TODO Set static constexpr
	int lives = 3;
	int direction = 0;					// TODO Clarify, consider an enum instead
	int activeTexture = 0;				// TODO Clarify, if this is the texture index used, name it so. Review usage and consider enum
	float timer = 0;

	EntityType type = EntityType::PLAYER;		// TODO What is this for? Refactor away

	void Initialize();
	void Render(Texture2D texture);
	void Update();
	
};


struct Projectile
{
public: 
	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = {0,0};
	int speed = 15; 
	bool active = true;  
	EntityType type = {};

	// LINE WILL UPDATE WITH POSITION FOR CALCULATIONS
	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

	void Update();

	void Render(Texture2D texture);
};

struct Wall 
{
public: 
	Vector2 position; 
	Rectangle rec; 
	bool active; 
	Color color; 
	int health = 50;
	int radius = 60;


	void Render(Texture2D texture); 
	void Update(); 
};

struct Alien
{
public:
	
	Color color = WHITE; 
	Vector2 position = {0, 0};
	int x = 0; 
	int y = 0; 
	float radius = 30;
	bool active = true;  
	bool moveRight = true; 
	
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

struct Game
{
	// Gamestate					// TODO Remove redudant comments
	State gameState = {};

	// Score
	int score;						// TODO Add default 

	// for later, make a file where you can adjust the number of walls (config file)	// TODO Consider, do or remove
	int wallCount = 5;

	//Aliens shooting				// TODO Clarify comment, then if logic already is self-explanatory, remove
	float shootTimer = 0;

	//Aliens stuff? (idk cause liv wrote this)
	Rectangle rec = { 0, 0 ,0 ,0 };				// TODO Rename to clarify

	int formationWidth = 8;						// TODO Set static constexpr for all these
	int formationHeight = 5;
	int alienSpacing = 80;
	int formationX = 100;
	int formationY = 50;

	bool newHighScore = false;					// TODO Consider renaming to "queueNewHighscore" or "newHighscoreTip"
	
	// TODO apply const, constexpr and noexcept where applicable
	void Start();						// TODO Rename to clarify what these do 
	void End();

	void Continue();					// TODO Rename to clarify transition to start screen
	void Launch();						// TODO Refactor away

	void Update();
	void Render();

	void SpawnAliens();

	bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineTop, Vector2 lineBottom);

	bool CheckNewHighScore();			

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



	Vector2 playerPos;
	Vector2 alienPos; 
	Vector2 cornerPos;
	float offset;



	//TEXTBOX ENTER
	char name[9 + 1] = "\0";      //One extra space required for null terminator char '\0'
	int letterCount = 0;

	Rectangle textBox = { 600, 500, 225, 50 };
	bool mouseOnText = false;

	int framesCounter = 0;

};