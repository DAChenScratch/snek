#include <string>
#include <vector>
#include <assert.h>
#include "snake.h"

using namespace std;

class GameBoard {
public:
	vector<vector<int>> board;
	vector<vector<bool>> visited;
	GameBoard();
	GameBoard(int width, int height);
	int getCoord(Point p);
	bool isValid(Point p);
	void print();
	void markVisited(Point p);
	bool isVisited(Point p);
	void clearVisited();
	GameBoard( const GameBoard & obj);
	void clearBoard();
};

GameBoard::GameBoard(const GameBoard & obj)
	:board(obj.board) , visited(obj.visited)
{
}

GameBoard::GameBoard() {
	board = vector<vector<int>>();
	visited = vector<vector<bool>>();
}

GameBoard::clearBoard(){
	for(auto y = board.begin(); y != board.end(); y++)
		for(auto x = (*y).begin(); x != (*y).end(); x++){
			if(x == (*y).begin() )

		}
	}
		if (i == 0 || i == height + 1 || j == 0 || j == width + 1 ) {
			tmp.push_back(WALL);
		} else {
			tmp.push_back(EMPTY);
		}
	}

}
}

GameBoard::GameBoard(int width, int height) {
	board = vector<vector<int>>(height + 2)(width + 2);
	visited = vector<vector<bool>>(height + 2)(width + 2);

}
int GameBoard::getCoord(Point p) {
	assert((p.y >= 0) && (p.y <= board.size()));
	assert((p.x >= 0) && (p.x <= board[p.y].size()));
	return board[p.y][p.x];
}

bool GameBoard::isValid(Point p) {
	if (!((p.y >= 0) && (p.y <= board.size() ))) {
		return false;
	}
	if (!((p.x >= 0) && (p.x <= board[p.y].size()))) {
		return false;
	}
	return board[p.y][p.x] != WALL && board[p.y][p.x] != BUFFER && board[p.y][p.x] < 0;
}

void GameBoard::print() {
	for (auto v : board) {
		for (auto i : v) {
			printf("%3d", i);
		}
		cout << endl;
	}
}

void GameBoard::markVisited(Point p) {
	visited[p.y][p.x] = true;
}
bool GameBoard::isVisited(Point p) {
	return  visited[p.y][p.x];
}

void GameBoard::clearVisited() {
	for (auto &vec : visited) {
		fill(vec.begin(), vec.end(), 0);
	}
}
