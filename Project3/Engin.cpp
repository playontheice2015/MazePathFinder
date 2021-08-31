#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
 
#include <windows.h>


namespace Engine {
	class Board;
	class Tile
	{
	public:
		int x, y;
		bool isWall = false;
		float f = 0;
		float g = 0;
		float h = 0; //heuristic
		bool isVisited = false;
		Board* board;
		void initNeighbours();
		std::vector<Tile*> neighbours;
		Tile* cameFrom;
		Tile(Board* board);
		Tile();
		~Tile();
	};
}

namespace Engine {
	class APathFinder
	{
	public:
		APathFinder(Board* board);
		~APathFinder(
		);

		/**
		APathFinder(Tile* start, Tile* target){
		m_start = start;
		m_target = target;
		m_board = start->board;
		}
		*/
		int findPath();
		int getFoundPaths(Tile * lastPoint, int * pOutBuffer, const int nOutBufferSize);
		Board* board;
	private:
		std::vector<Tile*> searchList;
		std::vector<Tile*> closedList;
		Tile* previousTile;
		bool contains(std::vector<Tile*> list, Tile* element);
		float heuristicCalc(Tile* startPoint, Tile* targetPoint);
		void eraseFromList(std::vector<Tile*> &list, Tile* element);
	};

}

namespace Engine {
	class Board
	{

	public:
		Tile** tiles;
		size_t rows;
		size_t cols;
		void draw();
		void init(const int nStartX, const int nStartY,
			const int nTargetX, const int nTargetY,
			const unsigned char pMap[], const int nMapWidth, const int nMapHeight,
			int* pOutBuffer, const int nOutBufferSize
		);

		Board();
		~Board();

		Tile* start;
		Tile* target;
		int pathCount=0;
		int* pOutBuffer;
		int nOutBufferSize;
	protected:
		bool contains(int* arrPoints, int nPoint);
		void setDetails(const int nStartX, const int nStartY,
			const int nTargetX, const int nTargetY,
			const unsigned char pMap[]);
	};


}

namespace Engine {
	APathFinder::APathFinder(Board* board) : board(board)
	{
		searchList.push_back(board->start);
	}

	APathFinder::~APathFinder()
	{}

	int APathFinder::findPath() {
		board->draw();

		if (this->board == nullptr)
			return -1;
		else if (board->start->x == board->target->x &&
			board->start->y == board->target->y)
			return 0;

		previousTile = this->board->start;
		this->board->start->cameFrom = this->board->start;
		while (searchList.size() > 0)
		{ 
				int topInx = 0;
				for (size_t i = 0; i < searchList.size(); i++)
				{
					if (searchList[i]->f < searchList[topInx]->f)
						topInx = i;
					// if (searchList[i]->f == searchList[topInx]->f) {
					//	// if f is equal thn find closer g
					//	if (searchList[i]->g > searchList[topInx]->g) {
					//		topInx = i;
					//	}
					//}
					 
				}
				Tile* currentTile = searchList[topInx];
				previousTile = currentTile;
/*				board->start = currentTile;
				board->draw()*/;
				/*	board->pathCount++;
				board->pOutBuffer[]*/
				eraseFromList(searchList, currentTile);
				closedList.push_back(currentTile);			

				if (currentTile == board->target) {
					std::cout << "\n Path to target is found!";
					return getFoundPaths(currentTile, board->pOutBuffer, board->nOutBufferSize);
				}
 
				currentTile->initNeighbours();

				for (size_t i = 0; i < currentTile->neighbours.size(); i++) {
					Tile* neighbour = currentTile->neighbours[i];
					if (!neighbour->isWall && !contains(closedList, neighbour)) {
						auto tmpG = currentTile->g + 1;
						bool newPath = false;

							
						// check if we try this before
							if (contains(searchList, neighbour)) {
								if (tmpG < neighbour->g) {
									neighbour->g = tmpG;
									newPath = true;
								}
							} else {
								neighbour->g = tmpG;
								newPath = true;
								searchList.push_back(neighbour);
							}
					
							if (newPath) {
								neighbour->h = heuristicCalc(neighbour, board->target);
								neighbour->f = neighbour->g + neighbour->h;
								neighbour->cameFrom = currentTile;
							}
					}
				}
			
		}
		return -1;
	};

	// setting found points to "outbutbuffer" and points count 
	// finding connected nodes by cameFom recursively from lastPoint
	int APathFinder::getFoundPaths(Tile* lastPoint, int* pOutBuffer, const int nOutBufferSize) {
		Tile* tmp = lastPoint;
		std::vector<int> pathList;
		pathList.push_back(tmp->x + (tmp->y * tmp->board->cols));

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD pos = { tmp->x, tmp->y +3};
		SetConsoleCursorPosition(hConsole, pos);
		WriteConsole(hConsole, ".", 1, NULL, NULL);
		Sleep(0.5 * 100);

		while (tmp->cameFrom != lastPoint->board->start)
		{
			if (tmp != nullptr) {
				tmp = tmp->cameFrom;
				pathList.push_back(tmp->x + (tmp->y * tmp->board->cols));
			}
			 ;
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			COORD pos = { tmp->x , tmp->y + 3 };
	
			SetConsoleTextAttribute(hConsole, 180);
			SetConsoleCursorPosition(hConsole, pos);
			
			WriteConsole(hConsole, ".", 1, NULL, NULL);
			Sleep(0.02 * 100);
		}

		
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			COORD pos = { 0, tmp->board->rows + 2 };
			SetConsoleCursorPosition(hConsole, pos);
			WriteConsole(hConsole, "r:", 1, NULL, NULL);
			Sleep(0.02 * 100);
		}


		int j = 0;
		board->pathCount = pathList.size();
		while ( j < board->pathCount )
			pOutBuffer[j++] = pathList[board->pathCount - j - 1];


		return j;
	}


	bool APathFinder::contains(std::vector<Tile*> list, Tile* element)
	{
		return (std::find(list.begin(), list.end(), element) != list.end());
	}

	void APathFinder::eraseFromList(std::vector<Tile*> &list, Tile* element)
	{
		auto e = std::find(list.begin(), list.end(), element);
		if (e != list.end()) {
			list.erase(e);
		}
	}

	// Manhaten block style
	// in future: if diagnol neighbours needed this method should should dev.
	float APathFinder::heuristicCalc(Tile * startPoint, Tile * targetPoint)
	{

		return ((double)sqrt((startPoint->x - targetPoint->x)*(startPoint->x - targetPoint->x)
			+ (startPoint->y - targetPoint->y)*(startPoint->y - targetPoint->y)));
		//return abs(startPoint->x - targetPoint->x) + abs(startPoint->y - targetPoint->y);
	}
}

namespace Engine {
	Board::Board()
	{}

	Board::~Board()
	{
		delete[] tiles;
	}

	void Board::init(
		const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		const unsigned char pMap[],
		const int nMapWidth, const int nMapHeight,
		int* pOutBuffer, const int nOutBufferSize) {
		this->cols = nMapWidth;
		this->rows = nMapHeight;
		this->tiles = new Tile*[rows];
		for (int row = 0; row < rows; row++) {
			tiles[row] = new Tile[cols];
		}
		this->pOutBuffer = pOutBuffer;
		this->nOutBufferSize = nOutBufferSize;
		setDetails(nStartX, nStartY, nTargetX, nTargetY, pMap);
	}

	void Board::setDetails(const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		const unsigned char pMap[]) {
		int arrayInx = 0;
		this->start = &(tiles[nStartY][nStartX]);
		this->target = &(tiles[nTargetY][nTargetX]);

		for (size_t r = 0; r < this->rows; r++)
			for (size_t c = 0; c < this->cols; c++)
			{
				tiles[r][c].x = c;
				tiles[r][c].y = r;
				tiles[r][c].isWall = !pMap[arrayInx];
				tiles[r][c].board = this;
				arrayInx++;
			}

		this->target = &(tiles[nTargetY][nTargetX]);
		target->isWall = false;

	}

	void Board::draw()
	{
		
		std::cout << "\n Map size " << rows << "x" << cols << std::endl;
		size_t arrayInx = 0;
		for (size_t r = 0; r < this->rows; r++)
		{
			for (size_t c = 0; c < this->cols; c++)
			{
				if (start->y == r && start->x == c) {
					std::cout << ">" ;
				}
				else if (target->y == r && target->x == c) {
					std::cout << "$";
				}
				else if (contains(this->pOutBuffer, c + (r*cols))) {
					std::cout << "." ;
				}
				else {
					std::cout << ((tiles[r][c].isWall) ? "#": " ");
				}
				arrayInx++;
			}
			std::cout << "\n";
		}
		for (size_t i = 0; i < pathCount; i++)
		{
			if (pOutBuffer[i]>-1)
				std::cout << pOutBuffer[i] << ",";
		}
	}


	bool Board::contains(int* arrPoints, int nPoint)
	{
		if (arrPoints == nullptr)
			return false;
		int n = sizeof(arrPoints) / sizeof(arrPoints[0]);
		auto it = std::find(arrPoints, arrPoints + n, nPoint);
		for (size_t i = 0; i < nOutBufferSize; i++)
		{
			if (arrPoints[i] == nPoint)
				return true;
		}
		return (it != (arrPoints + n));
	}
}

namespace Engine {
	Tile::Tile(Board* board) : board(board) {}

	Tile::Tile()
	{
	}

	Tile::~Tile()
	{
	}

	void Tile::initNeighbours()
	{
		if (this->neighbours.size() > 0)
			return;

		int col = x;
		int row = y;
		int cols = board->cols;
		int rows = board->rows;

		if (col > 0)
			this->neighbours.push_back(&(this->board->tiles[row][col - 1]));
		if (col < cols - 1)
			this->neighbours.push_back(&(this->board->tiles[row][col + 1]));
		if (row < rows - 1)
			this->neighbours.push_back(&(this->board->tiles[row + 1][col]));
		if (row > 0)
			this->neighbours.push_back(&(this->board->tiles[row - 1][col]));
	}
}

namespace engine {
	///FindPath by using A*
	int FindPath(
		const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		const unsigned char pMap[],
		const int nMapWidth, const int nMapHeight,
		int* pOutBuffer, const int nOutBufferSize)
	{

		auto board = Engine::Board();
		board.init(nStartX, nStartY, nTargetX, nTargetY,
			pMap, nMapWidth, nMapHeight, pOutBuffer, nOutBufferSize);
		//board.draw(); // plain draw to see the map board

		auto pathFinder = new Engine::APathFinder(&board);
		auto res = pathFinder->findPath();
		//board.draw(); // result draw to see path

		std::cout << "\nA* path search result:" << res << std::endl;

		delete pathFinder;
		return res;
	}
}
 
int FindPath(
	const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char pMap[],
	const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize) {	 

	int res = engine::FindPath(nStartX, nStartY, nTargetX, nTargetY,
		pMap, nMapWidth, nMapHeight, pOutBuffer, nOutBufferSize);
	 
	return res;
}

//****************************************************************************///
#include <iostream>
#include "time.h"

using namespace std;
//----CONSTANTS-------------------------------------------------------
#define GRID_WIDTH 110
#define GRID_HEIGHT 23
#define NORTH 0
#define EAST 1  
#define SOUTH 2
#define WEST 3

namespace engine {
 
	char grid[GRID_WIDTH*GRID_HEIGHT];
	//----FUNCTION PROTOTYPES---------------------------------------------
	void ResetGrid();
	int XYToIndex(int x, int y);
	int IsInBounds(int x, int y);
	void Visit(int x, int y);
	void PrintGrid();
	//----FUNCTIONS-------------------------------------------------------
	int mainXX()
	{
		// Starting point and top-level control.
		srand(time(0)); // seed random number generator.
		ResetGrid();
		Visit(1, 1);
		PrintGrid();
		cin.get();
		return 0;
	}

	 char* GenareteMaze() {
		srand(time(0)); // seed random number generator.
		ResetGrid();
		Visit(1, 1);
		//PrintGrid();
		 

		return grid;
	}

	void ResetGrid()
	{
		for (int i = 0; i < GRID_WIDTH*GRID_HEIGHT; ++i)
		{
			grid[i] = '0';
		}
	}
	int XYToIndex(int x, int y)
	{
		return y * GRID_WIDTH + x;
	}
	int IsInBounds(int x, int y)
	{
		if (x < 0 || x >= GRID_WIDTH) return false;
		if (y < 0 || y >= GRID_HEIGHT) return false;
		return true;
	}
	void Visit(int x, int y)
	{
		grid[XYToIndex(x, y)] = '1';
		int dirs[4];
		dirs[0] = NORTH;
		dirs[1] = EAST;
		dirs[2] = SOUTH;
		dirs[3] = WEST;
		for (int i = 0; i < 4; ++i)
		{
			int r = rand() & 3;
			int temp = dirs[r];
			dirs[r] = dirs[i];
			dirs[i] = temp;
		}

		for (int i = 0; i < 4; ++i)
		{
			// dx,dy are offsets from current location. Set them based
			// on the next direction I wish to try.
			int dx = 0, dy = 0;
			switch (dirs[i])
			{
			case NORTH: dy = -1; break;
			case SOUTH: dy = 1; break;
			case EAST: dx = 1; break;
			case WEST: dx = -1; break;
			}

			int x2 = x + (dx << 1);
			int y2 = y + (dy << 1);
			if (IsInBounds(x2, y2))
			{
				if (grid[XYToIndex(x2, y2)] == '0')
				{
					grid[XYToIndex(x2 - dx, y2 - dy)] = '1';
					// Recursively Visit (x2,y2)
					Visit(x2, y2);
				}
			}
		}
	}
	void PrintGrid()
	{
		for (int y = 0; y < GRID_HEIGHT; ++y)
		{
			for (int x = 0; x < GRID_WIDTH; ++x)
			{
				cout << grid[XYToIndex(x, y)];
			}
			cout << endl;
		}
	}
}
//****************************************************************************///


int main() {
	static const int height = GRID_HEIGHT;
	static const int width = GRID_WIDTH;

	char* pMapChar = engine::GenareteMaze();

	unsigned char pMap[GRID_WIDTH*GRID_HEIGHT];
	for (int i = 0; i < GRID_WIDTH*GRID_HEIGHT; i++)
	{
		pMap[i] = pMapChar[i] == '1' ? 1 : 0;
	}
		 
		int pOutBuffer[width * height];
		clock_t start, end;
		start = clock();
		auto t1 = std::chrono::high_resolution_clock::now();
		std::cin.get();
		int strtX = 1, strtY = 1;
		int targetX = width-2, targetY= height-2;

		int a = FindPath(strtX, strtY , targetX, targetY, pMap, width, height, pOutBuffer, width*height);

		auto t2 = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

		float dur = duration / 10000;
		std::cout << "\n duraction:" << duration / 1000;

		std::cout << a;
		std::cin.get();
		return 0;
	}