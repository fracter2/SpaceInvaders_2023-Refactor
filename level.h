#pragma once 
#include <iostream>											// TODO Remove unused headers
#include <string>
#include <fstream>


struct Entity												// TODO Move to Game.h or rename file to better reflect that this only contains Entity struct
{
	float x, y;

	void LoadLevelFromAFile(const std::string& filename);	// TODO Consider implementing (from the similarily-named func in level.cpp) or remove. Unused
															// TODO string should be string_view
};