#pragma once
#include <vector>;


/*

1. get cells displayed in uniform grid
2. 


*/

using namespace std;

struct Cell
{
	int myX;
	int myY;
	bool southWall;
	bool eastWall;
	bool bVisited = false;
};

class Maze
{
public:
	unsigned int height;
	unsigned int width;
	vector<vector<Cell>> cells;
	vector<Cell*> checkCells;
	Cell* nextCell;

public:
	Maze();
	~Maze();

	void Generate();
	bool Maze::GetUnvisitedCell(Cell* cell);
	void VisitCell(Cell *cell);
	Cell* GetCell(int x, int y);
};

