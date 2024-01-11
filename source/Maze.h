#pragma once
#include <vector>;

/*

1. get cells displayed in uniform grid
2. 


*/

using namespace std;

struct Cell
{
	bool bVisited = false;
};

class Maze
{
public:
	unsigned int height;
	unsigned int width;
	vector<vector<Cell>> cells;

public:
	Maze();
	~Maze();

	void Generate();
};

