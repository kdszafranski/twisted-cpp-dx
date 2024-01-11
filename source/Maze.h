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
	bool bVisited = false;
};

class Maze
{
public:
	unsigned int height;
	unsigned int width;
	vector<vector<Cell>> cells;
	vector<Cell> checkCells;

public:
	Maze();
	~Maze();

	void Generate();
	void VisitCell(Cell *cell);
	Cell GetCell(int x, int y);
};

