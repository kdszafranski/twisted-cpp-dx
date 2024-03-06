#include "Maze.h"
#include <time.h>

Maze::Maze()
{
	height = 10;
	width = 10;
	cells.clear();
	nextCell = nullptr;

	// 0 equals wall
	// 1 equals traversable
	
	// rows
	for (int i = 0; i < width; i++) {
		// cols		
		// create new vector
		std::vector<Cell> vec;
		// fill new vector
		for (int j = 0; j < height; j++) {
			Cell cell;
			cell.myX = i;
			cell.myY = j;
			cell.southWall = true;
			cell.eastWall = true;
			cell.bVisited = false;

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

/*
Choose the initial cell, mark it as visited and push it to the stack
While the stack is not empty
	Pop a cell from the stack and make it a current cell
	If the current cell has any neighbours which have not been visited
	Push the current cell to the stack
	Choose one of the unvisited neighbours
	Remove the wall between the current cell and the chosen cell
	Mark the chosen cell as visited and push it to the stack
*/
void Maze::Generate()
{
	Cell* someCell = GetCell(0,0);
	someCell->bVisited = true;

	// Pop a cell from the stack and make it a current cell
	checkCells.push_back(someCell);
	srand(time(NULL));

	while (checkCells.size() > 0) {
		// need a ref into this cell inside the master cell list
		Cell* currentCell = checkCells.back();
		checkCells.pop_back();

		// If the current cell has any neighbours which have not been visited
		int randNum = (rand() % 2) + 1;

		if (GetUnvisitedCell(currentCell)) {
			// true, so nextCell is populated with the next cell
			if (nextCell->bVisited == false) {
				currentCell->bVisited = true;
				// Remove the wall between the current cell and the chosen cell
				if (nextCell->myX - currentCell->myX == 0) {
					// we went north/south
					currentCell->southWall(false);
				}
				currentCell->eastWall = false;
				// Mark the chosen cell as visited and push it to the stack
				checkCells.push_back(nextCell);
			}
		}
	}
	
}

bool Maze::GetUnvisitedCell(Cell* cell) 
{
	// go in 4 directions and see if there is 
	nextCell = nullptr;

	// East
	if (cell->myX + 1 <= width) {
		nextCell = GetCell(cell->myX + 1, cell->myY);
		if (!nextCell->bVisited) {
			return true;
		}
	}
	// South
	if (cell->myY + 1 <= height) {
		nextCell = GetCell(cell->myX, cell->myY + 1);
		if (!nextCell->bVisited) {
			return true;
		}
	}

	return nullptr;
}

void Maze::VisitCell(Cell *cell)
{
	cell->bVisited = true;

}

Cell* Maze::GetCell(int x, int y)
{
	return &cells.at(x).at(y);
}

