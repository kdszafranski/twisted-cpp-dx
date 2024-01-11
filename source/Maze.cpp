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
			cell.myX = i;
			cell.myY = j;
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
	Cell someCell = GetCell(5, 6);
	checkCells.push_back(someCell);

	while (checkCells.size() > 0) {
		Cell* currentCell = &checkCells.back();

		// check right
		Cell* visit = GetCell(currentCell->myX + 1, currentCell->myY);
		if (visit) {
			if (visit->bVisited == false) {

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

Cell Maze::GetCell(int x, int y)
{
	return cells.at(x).at(y);
}

