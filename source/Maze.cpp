#include "Maze.h"

Maze::Maze()
{
	height = 10;
	width = 10;
	cells.clear();

	// 0 equals wall
	// 1 equals traversable
	
	// rows
	for (int i = 0; i < height; i++) {
		// cols		
		// create new vector
		std::vector<int> vec;
		// fill new vector
		for (int j = 0; j < width; j++) {
			vec.push_back(0);
		}
		// add filled vector to outer vector
		cells.push_back(vec);
	}
}

Maze::~Maze()
{ }

