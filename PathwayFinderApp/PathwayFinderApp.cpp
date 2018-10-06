//
// PathwayFinderApp.cpp : Quick and Dirty Program to find path between points A and B as per interview question
// 
// (c) 2018 Craig D. French


#include <string>
#include <vector>
#include <forward_list>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>
#include <iterator>

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

	bool friend IsInSet(const Coordinates& IsMember, const vector<Coordinates> list)
	{
		for (vector<Coordinates>::const_iterator i = list.begin(); i != list.end(); i++)
		{
			if (*i == IsMember)
			{
				return true;
			}
		}
		return false;
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

class WorkItem
{
public:
	WorkItem(Coordinates coord, vector<Coordinates> pathway) : current(coord), path(pathway)
	{
	}
	Coordinates current;
	vector<Coordinates> path;
};

/// <summary>
/// Utility class for managing data for the search grid. 
/// </summary>
class GridManager
{
public: 
	/// <summary>
	/// Find the height, width from the grid text array
	/// Locate the start [Aa] and end points [Bb]
	/// Initialize the binary visited bool array (linear)
	/// </summary>
	GridManager(vector<string> textArray) 
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
		
        allDirections.emplace_front(make_pair(-1,0));
        allDirections.emplace_front(make_pair(1,0));
        allDirections.emplace_front(make_pair(0,-1));
        allDirections.emplace_front(make_pair(0,1));
	}

	~GridManager()
	{
		delete visited;
	}

	bool InGrid(Coordinates item)
	{
		return (item.x >= 0 && item.y >= 0 && item.y < height && item.x < width);
	}
	
	// Simplfy the code by assuming that if we don't have data 
	// we will return a wall. 
	char Item(Coordinates item)
	{
		// If the data isn't there assume it is a wall
		if (item.x >= data[item.y].length())
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
	forward_list<pair<int,int>> AllDirections() { return allDirections; }
	void SetSolution(vector<Coordinates> solution)
	{
		solutionSet = solution;
	}
	void AddToWorkQueue(Coordinates c, vector<Coordinates> path) {		
		workQueue.push_back(WorkItem(c,path));
	}
	vector<WorkItem>DetachWorkQueue()
	{
		vector<WorkItem> returnValue = workQueue;
		workQueue.clear();
		return returnValue;
	}
	size_t WorkQueueSize() {
		return workQueue.size();
	}
	friend std::ostream& operator<<(std::ostream& stream, GridManager& val)
	{
		for (size_t row = 0; row < val.Height(); row++)
		{
			for (size_t column = 0; column < val.Width(); column++)
			{
				Coordinates current((int)column, (int)row);
				char itemLetter = val.Item(current);
				char letter = (itemLetter == ' ' && (IsInSet(current, val.solutionSet))) ? 'o' : itemLetter;
				cout << letter;
			}
			cout << endl;
		}
		return stream;
	}


private:
	vector<string> data;
	forward_list<pair<int, int>> allDirections;
	vector<Coordinates> solutionSet;
	vector<WorkItem> workQueue;
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


vector<Coordinates> ProcessWorkQueue(GridManager& grid)
{

	vector<WorkItem> workQueue = grid.DetachWorkQueue();
	for (auto work : workQueue)
	{
		work.path.push_back(work.current);
		grid.Visited(work.current);
		if (grid.EndingPoint() == work.current)
		{
			cout << "Found in workQueueItem: "s << endl;
			return work.path;
		}
		for (int index = 0; index < grid.AllDirections().size(); index++) {

			Coordinates target = grid.AllDirections()[index] + work.current;

			// If it in the grid parameters
			// is not a wall 
			// and has not been visited yet
			if ((grid.InGrid(target))
				&& ('X' != (grid.Item(target)))
				&& (!grid.BeenVisited(target)))
			{
				// Create a copy of the path so far
				vector<Coordinates> resultPath = work.path;

				// So the successful path can be recorded if it ends up being successful
				grid.AddToWorkQueue(target, resultPath);
			}
		}
	}
	workQueue.clear();
	return vector<Coordinates>();
}


int main(int argc, char** argv)
{
	vector<string> gridText;
    

	if (argc == 2)
	{
		std::ifstream is(argv[1]);
		if (is) {

			std::string line;
			while (std::getline(is, line))
			{
				std::cout << line << endl;
				gridText.push_back(line);
			}
		}
		else
		{
			char buffer[1000];
			//cerr << "Current directory: " << _getcwd(buffer, sizeof(buffer));
			cerr << "File: " << argv[1] << " could not be opened!\n"; // Report error
			cerr << "Error code: " << strerror_s(buffer, sizeof(buffer), errno) ; // Get some info as to why
		}

	}
	else
	{
		gridText.push_back("A        XX");
		gridText.push_back("          X");
		gridText.push_back("XXXXXX       XX             XXX    ");
		gridText.push_back("          XX      XXX                                      B   ");
	}

	GridManager grid(gridText);

	cout << grid;
	cout << "StartPoint is: " << grid.StartingPoint() << endl;
	cout << "Endpoint is: " << grid.EndingPoint() << endl;
	
	std::vector<Coordinates> path, solvedPath;

	grid.AddToWorkQueue(grid.StartingPoint(), path);
	size_t level = 0;
	while (grid.WorkQueueSize() > 0)
	{
		solvedPath = ProcessWorkQueue(
			grid);
		if (solvedPath != grid.Null()) break;
		cout << "Depth: " << level++ << " WorkQueueSize:" << grid.WorkQueueSize() << endl;
	}
	cout << grid;
	// If we have a path, print it using the vector iterator
	if (!solvedPath.empty())
	{
		for (vector<Coordinates>::const_iterator i = solvedPath.begin(); i != solvedPath.end(); ++i)
		{
			cout << *i << endl;
		}
		grid.SetSolution(solvedPath);
		cout << grid;
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

