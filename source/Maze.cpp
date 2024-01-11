#include "Maze.h"

Maze::Maze()
{
	height = 10;
	width = 10;
	cells.clear();
	
	// rows
	for (int i = 0; i < height; i++) {
		// cols		
		// create new vector
		std::vector<int> vec;
		// fill new vector
		for (int j = 0; j < width; j++) {
			vec.push_back(j);
		}
		// add filled vector to outer vector
		cells.push_back(vec);
	}
}

Maze::~Maze()
{ }

