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
	~GameBoard();
};

GameBoard::~GameBoard(){
	board.clear();
	visited.clear();
}

GameBoard::GameBoard(const GameBoard & obj)
	:board(obj.board) , visited(obj.visited)
{
}

GameBoard::GameBoard() {
}

void GameBoard::clearBoard(){
	for(auto y = board.begin(); y != board.end(); y++){
		for(auto x = y->begin(); x != y->end(); x++){
			if(x == y->begin() || x == y->end() - 1 || y == board.begin() || y == board.end() - 1){
				*x = WALL;
			}else{
				*x = EMPTY;
			}
		}
	}
}

GameBoard::GameBoard(int width, int height) {
	board.resize(height + 2);
	visited.resize(height + 2);
	for(int i = 0; i < height + 2; i++){
		board[i].resize(width + 2);
		visited[i].resize(width + 2);
	}
	clearBoard();
}

int GameBoard::getCoord(Point p) {
	assert((p.y >= 0) && (p.y <= board.size()));
	assert((p.x >= 0) && (p.x <= board[p.y].size()));
	return board[p.y][p.x];
}

bool GameBoard::isValid(Point p) {
	if (!((p.y >= 0) && (p.y < board.size() ))) {
		return false;
	}
	if (!((p.x >= 0) && (p.x < board[p.y].size()))) {
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
