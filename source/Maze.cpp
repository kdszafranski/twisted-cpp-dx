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
		std::vector<Cell> vec;
		// fill new vector
		for (int j = 0; j < width; j++) {
			Cell cell;
			vec.push_back(cell);
		}
		// add filled vector to outer vector
		cells.push_back(vec);
	}
}

Maze::~Maze()
{ 
	cells.clear();
}

void Maze::Generate()
{
	Cell *currentCell; // ref only so we can change the source struct

	for (int i = 0; i < height; i++) {
		// cols		
		for (int j = 0; j < width; j++) {
			currentCell = &cells.at(i).at(j);

			// every other cell
			if (j % 2 == 0) {
				currentCell->bVisited = true;
			}

		}
	}
}

