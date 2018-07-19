//
// PathwayFinderApp.cpp : Quick and Dirty Program to find path between points A and B as per interview question
// 
// (c) 2018 Craig D. French

#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

/// <summary>
/// Super simple coordinates class to make things easier.
/// </summary>
class Coordinates
{
	public:
	Coordinates(int setX = 0, int setY = 0)
		: x(setX), y(setY) {}
	int x;
	int y;

	bool operator==(const Coordinates& a)
	{
		return ((a.x == x) && (a.y == y));
	}

	Coordinates& operator+= (const Coordinates& a)
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	friend Coordinates operator+ (const Coordinates& a, const Coordinates& b)
	{
		return Coordinates( a.x + b.x, a.y + b.y);
	}


	Coordinates& operator= (const Coordinates& c)
	{
		x = c.x;
		y = c.y;
		return *this;
	}

	bool friend operator==(const Coordinates& a, const Coordinates& b)
	{
		return (a.x == b.x && a.y == b.y);
	}

	friend std::ostream& operator<<(std::ostream& stream, const Coordinates& val)
	{
		return stream << "(" << val.x << "," << val.y << ")";
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
	GridObject(vector<string> textArray)
	{
		data = textArray;
		height = textArray.size();
		width = 0;
		for (int row = 0; row< height; row++)
		{
			size_t length = (data[row]).length();
			FindPoints(data[row], row);
			if (length > width)
			{
				width = length;
			}
		}
		visited = new bool[width*height];
		
		for (int i = 0; i < width*height; i++)
		{
			visited[i] = false;
		}
        allDirections.push_back(Coordinates(-1,0));
        allDirections.push_back(Coordinates(1,0));
        allDirections.push_back(Coordinates(0,-1));
        allDirections.push_back(Coordinates(0,1));
	}

	~GridObject()
	{
		delete visited;
	}

	bool InGrid(Coordinates item)
	{
		return (!(item.x < 0 || item.y < 0 || item.y >= height || item.x >= width ));
	}
	
	// Simplfy the code by assuming that if we don't have data 
	// we will return a wall. 
	char Item(Coordinates item)
	{
		// If the data isn't there assume it is a wall
		if (!InGrid(item) && (item.x >= data[item.y].length()))
		{
			return 'X';
		}
		else
		{
			return data[item.y].substr(item.x,1)[0];
		}
	}

	bool BeenVisited(Coordinates item)
	{
		return visited[item.y*width + item.x];
	}
	void Visited(Coordinates item)
	{
		visited[item.y*width + item.x] = true;
	}

	size_t Width() { return width;  }
	size_t Height() { return height; }
	Coordinates StartingPoint(){ return startingPoint; }
	Coordinates EndingPoint() { return endingPoint; }
	vector<Coordinates> Null() { return (vector<Coordinates>)NULL; }
	vector<Coordinates> AllDirections() { return allDirections; }

private:
	vector<string> data;
	vector<Coordinates> allDirections;
	size_t height;
	size_t width;
	Coordinates startingPoint;
	Coordinates endingPoint;
	bool *visited;

private:
	// Helper function to locate startPoint [Aa] and endingPoint [Bb] 
	void FindPoints(string str, int yCoord)
	{
		size_t foundAt = 0;
		while (foundAt != string::npos)
		{
			foundAt = str.find_first_of("AaBb", foundAt);
			if ( string::npos != foundAt )
			{
				Coordinates *point = 
					('A' == toupper(str[foundAt])) ? 
					&startingPoint : &endingPoint;

				point->x = (int)foundAt;
				point->y = yCoord;

				// Continue searching at next character
				foundAt++;
			}
		}
	}
};

/// <summary>
/// Traverses in different directions until it hits a wall or the edge
///	Returns null if no path, otherwise the first path found that works
/// </summary>
vector<Coordinates> traverseInAllDirections(Coordinates current, GridObject *grid, vector<Coordinates> path)
{
	path.push_back(current);

	grid->Visited(current);

	// We have a match!!!
	if (grid->EndingPoint()==current)
	{
		return path;
	}

	// Set return value
	vector<Coordinates> results = grid->Null();

	for (int index=0; index < grid->AllDirections().size(); index++)
	{
		Coordinates target = grid->AllDirections()[index] + current;

		// If it in the grid parameters
		// is not a wall 
		// and has not been visited yet
		if ((grid->InGrid(target)) 
			&& ('X' != (grid->Item(target))) 
			&& (!grid->BeenVisited(target)))
		{
			// Create a copy of the path so far
			vector<Coordinates> resultPath = path; 

			// So the successful path can be recorded if it ends up being successful
			results = traverseInAllDirections(target, grid, resultPath);

			// Return immediately to caller since we have a match
			if (results != grid->Null()) break;
		}
	}
	return results;
}


int main()
{
	static vector<string> gridText;
    
	gridText.push_back("       X B  ");
	gridText.push_back("   A   X    ");
    gridText.push_back("            ");
	gridText.push_back("       XXXXX");

	GridObject grid(gridText);

	cout << "StartPoint is: " << grid.StartingPoint() << endl;
	cout << "Endpoint is: " << grid.EndingPoint() << endl;

	vector<Coordinates> path, solvedPath;

	solvedPath = traverseInAllDirections(
		grid.StartingPoint(),
		&grid, path);

	// If we have a path, print it using the vector iterator
	if (solvedPath != grid.Null())
	{
		for (vector<Coordinates>::const_iterator i = solvedPath.begin(); i != solvedPath.end(); ++i)
		{
			cout << *i << endl;
		}
	}
	else
	{
		cout << "Unable to find solution" << endl;
	}

#ifndef __EMSCRIPTEN__
	cout << "Press enter to continue" << endl;
	string read;
	std::getline(cin, read);
#endif
}

