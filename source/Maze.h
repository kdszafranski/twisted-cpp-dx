#pragma once
#include <vector>;

/*

1. get cells displayed in uniform grid
2. 


*/

using namespace std;

class Maze
{
public:
	unsigned int height;
	unsigned int width;
	vector<vector<int>> cells;

public:
	Maze();
	~Maze();
};

