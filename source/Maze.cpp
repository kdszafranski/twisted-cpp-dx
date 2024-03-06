#include "Maze.h"

Maze::Maze()
{
	height = 10;
	width = 10;
	cells.clear();

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

	while (checkCells.size() > 0) {
		// need a ref into this cell inside the master cell list
		Cell* currentCell = checkCells.back();
		checkCells.pop_back();

		// If the current cell has any neighbours which have not been visited
		// 
		// check right
		int dir = 2; // east

		// Choose one of the unvisited neighbours
		if (currentCell->myX + 1 < width) {
			Cell* visit = GetCell(currentCell->myX + 1, currentCell->myY);
			if (visit->bVisited == false) {
				currentCell->bVisited = true;
				// Remove the wall between the current cell and the chosen cell
				currentCell->eastWall = false;
				// Mark the chosen cell as visited and push it to the stack
				checkCells.push_back(visit);
			}
		}
	}
	

	//for (int i = 0; i < height; i++) {
	//	// cols		
	//	for (int j = 0; j < width; j++) {
	//		currentCell = &cells.at(i).at(j);
	//		


	//	}
	//}
}

void Maze::VisitCell(Cell *cell)
{
	cell->bVisited = true;

}

Cell* Maze::GetCell(int x, int y)
{
	return &cells.at(x).at(y);
}

