//
// PathwayFinderApp.cpp : Quick and Dirty Program to find path between points A and B as per interview question
// 
// (c) 2018 Craig D. French

#include "stdio.h"
#include "conio.h"
#include "string.h"
#include "ctype.h"
#include <string>
#include <vector>

using namespace std;

static char *gridText[] = {
	"       X B  ",
	"   A   X   ",
	"           ",
	"       XXXXX",
	NULL,
};

/// <summary>
/// Super simple coordinates class to make things easier.
/// </summary>
class Coordinates
{
public:

	Coordinates::Coordinates(int setX = 0, int setY = 0)
		: x(setX), y(setY) {}
	int x;
	int y;

	bool Coordinates::operator==(Coordinates& a)
	{
		return ((a.x == x) && (a.y == y));
	}

	Coordinates& Coordinates::operator+= (Coordinates& a)
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	char *Coordinates::toString(char *buffer) {
		sprintf(buffer, "(%d,%d)", x, y);
		return buffer;
	}
};

/// <summary>
/// Utility class for managing data for the search grid. 
/// </summary>
class GridObject
{
public: 
	/// <summary>
	/// Find the height, width from the grid text array
	/// Locate the start [Aa] and end points [Bb]
	/// Initialize the binary visited bool array (linear)
	/// </summary>
	GridObject::GridObject(char **array)
	{
		data = array;
		char **str = data;
		height = 0;
		width = -1;
		while (*str != NULL)
		{
			int length = strlen(*str);
			FindPoints(*str);
			if (length > width)
			{
				width = length;
			}
			str++;
			height++;
		}
		visited = new bool[width*height];
		
		for (int i = 0; i < width*height; i++)
		{
			visited[i] = false;
		}
		
	}

	GridObject::~GridObject()
	{
		delete visited;
	}

	bool GridObject::InGrid(Coordinates& item)
	{
		return (!(item.x < 0 || item.y < 0 || item.y >= height || item.x >= width ));
	}
	
	// Simplfy the code by assuming that if we don't have data 
	// we will return a wall. 
	char GridObject::Item(Coordinates item)
	{
		// If the data isn't there assume it is a wall
		if (!InGrid(item) && (item.x >= (int)strlen(data[item.y])))
		{
			return 'X';
		}
		else
		{
			return *(data[item.y] + item.x);
		}
	}

	bool GridObject::BeenVisited(Coordinates item)
	{
		return visited[item.y*width + item.x];
	}
	void GridObject::Visited(Coordinates item)
	{
		visited[item.y*width + item.x] = true;
	}

	int Width() { return width;  }
	int Height() { return height; }
	Coordinates StartingPoint(){ return startingPoint; }
	Coordinates EndingPoint() { return endingPoint; }

private:
	char **data;
	int height;
	int width;
	Coordinates startingPoint;
	Coordinates endingPoint;
	bool *visited;

private:
	// Helper function to locate startPoint [Aa] and endingPoint [Bb] 
	void GridObject::FindPoints(char *row)
	{
		string str = string(row);
		size_t foundAt = 0;
		while (foundAt != string::npos)
		{
			foundAt = str.find_first_of("AaBb", foundAt);
			if ( string::npos != foundAt )
			{
				Coordinates *point = 
					('A' == toupper(str[foundAt])) ? 
					&startingPoint : &endingPoint;

				point->x = foundAt;
				point->y = height;

				// Continue searching at next character
				foundAt++;
			}
		}
	}
};

/// <summary>
/// Directions to traverse looking for the match
/// </summary>
/// <remarks>
/// Try the search paths that move across the grid
/// first in the hope that it will find the target
/// faster.
/// </remarks>
static vector<Coordinates> allDirections = {
	Coordinates(-1,-1),
	Coordinates(1,1),
	Coordinates(1,-1),
	Coordinates(-1,1),
	Coordinates(-1,0),
	Coordinates(1,0),
	Coordinates(0,-1),
	Coordinates(0,1),
};

/// <summary>
/// Traverses in 8 different directions until it hits a wall or the edge
///	Returns null if no path, otherwise the first path found that works
/// </summary>
vector<string> traverseInAllDirections(Coordinates& current, GridObject *grid, vector<string> path)
{
	char buffer[100];
	sprintf(buffer, "(%d,%d)", current.x, current.y);
	path.push_back(buffer);

	grid->Visited(current);

	// We have a match!!!
	if (grid->EndingPoint()==current)
	{
		return path;
	}

	// Set return value
	vector<string> results = (vector<string>)NULL;

	// Get a copy of the directions vector which we will 
	// add to the current coordinates to determine our 
	// approach 
	vector <Coordinates> targetPositions = allDirections;
	for (int i = 0; i < (int)targetPositions.size(); i++)
	{
		targetPositions[i] += current;

		// If it in the grid parameters
		// is not a wall 
		// and has not been visited yet
		if ((grid->InGrid(targetPositions[i])) 
			&& ('X' != (grid->Item(targetPositions[i]))) 
			&& (!grid->BeenVisited(targetPositions[i])))
		{
			// Create a copy of the path so far
			vector<string> resultPath = path; 

			// So the successful path can be recorded if it ends up being successful
			results = traverseInAllDirections(targetPositions[i], grid, resultPath);

			// Return immediately to caller since we have a match
			if (results != (vector<string>)NULL) break;
		}
	}
	return results;
}


int main()
{
	char buffer[40];
	GridObject grid(gridText);

	printf("StartPoint is %s\n", grid.StartingPoint().toString(buffer));
	printf("Endpoint is %s\n", grid.EndingPoint().toString(buffer));

	vector<string> path, solvedPath;

	solvedPath = traverseInAllDirections(
		grid.StartingPoint(),
		&grid, path);

	// If we have a path, print it using the vector iterator
	if (solvedPath != (vector<string>)NULL)
	{
		for (vector<string>::const_iterator i = solvedPath.begin(); i != solvedPath.end(); ++i)
		{
			printf("%s\n", (*i).c_str());
		}
	}
	else
	{
		printf("Unable to find solution");
	}
	printf("Press any key to continue");
		
	// Wait for keystroke when using the IDE so we can see
	while (!kbhit());
	getch();
}

