#include <string>
#include <iostream>
#include <vector>
#include "json/src/json.hpp"
#include "crow/amalgamate/crow_all.h"
#include <assert.h>
#include <queue>

using namespace std;
using namespace crow;
using JSON = nlohmann::json;

#define CLASSIC 0
#define ADVANCED 1

#define DEAD 0
#define ALIVE 1

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

const vector<int> moveslist = {NORTH, SOUTH, EAST, WEST};

#define EMPTY -1
#define WALL -2
#define FOOD -3
#define GOLD -4

#define DEFAULT 0
#define EAT 1
#define FINDFOOD 2



class Point {
public:
	int x;
	int y;
	Point();
	Point(int x1, int y1);
	Point addMove(int move);
	Point convert();
	vector<Point> expand();
};

class Path {
public:
	vector<Point> path;
	int target;
	Path();
	int getStepDir(int step);
};


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
};


class Snake {
public:
	string id;
	string name;
	int health;
	vector<Point> coords;
	Snake();
	Snake(JSON j);
	Point getHead();
};

class GameInfo {
public:
	string id;
	string game_id;
	int turn;
	int height;
	int width;
	vector<Snake> snakes;
	vector<Point> food;
	Snake snake;
	GameBoard board;
	GameInfo(string body);
	Path breadthFirstSearch(Point start, vector<int> targets, bool geq);
private:
	int parseMode(string str);
	void updateBoard();
	void getMySnake();
};


GameBoard::GameBoard() {
	board = vector<vector<int>>();
	visited = vector<vector<bool>>();
}

Point::Point(){
	x = 0;
	y = 0;
}

Point::Point(int x1, int y1) {
	x = x1;
	y = y1;
}

Point Point::addMove(int move) {
	assert(move == NORTH || move == EAST || move == WEST || move == SOUTH);
	Point p = Point(x, y);
	switch (move) {
	case NORTH:
		p.y--;
		break;
	case EAST:
		p.x++;
		break;
	case SOUTH:
		p.y++;
		break;
	case WEST:
		p.x--;
		break;
	}
	return p;
}
Point Point::convert() {
	return Point(x + 1, y + 1);
}

vector<Point> Point::expand() {
	vector<Point> moves = vector<Point>();
	for (auto m : moveslist) {
		Point p = addMove(m);
		moves.push_back(p);
	}
	return moves;
}


GameBoard::GameBoard(int width, int height) {
	board = vector<vector<int>>();
	for (int i = 0; i < height + 2; i++) {
		vector<int> tmp = vector<int>();
		vector<bool> vtmp = vector<bool>();
		for (int j = 0; j < width + 2; j++) {
			if (i == 0 || i == height + 1 || j == 0 || j == width + 1 ) {
				tmp.push_back(WALL);
			} else {
				tmp.push_back(EMPTY);
			}
			vtmp.push_back(false);
		}
		board.push_back(tmp);
		visited.push_back(vtmp);
	}
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
	return board[p.y][p.x] != WALL && board[p.y][p.x] < 0;
}

void GameBoard::print() {
	for (auto v : board) {
		for (auto i : v) {
			printf("%3d", i);
		}
		cout << endl;
	}
}

void GameBoard::markVisited(Point p){
	visited[p.y][p.x] = true;
}
bool GameBoard::isVisited(Point p){
	return  visited[p.y][p.x];
}

Snake::Snake() {
	coords = vector<Point>();
}

Snake::Snake(JSON j) {
	id = j["id"];
	name = j["name"];
	health = j["health_points"];
	for (auto p : j["coords"]) {
		coords.push_back(Point(p[0], p[1]).convert());
	}
}

Point Snake::getHead() {
	assert(coords.size() > 0);
	return coords[0];
}

GameInfo::GameInfo(string body) {
	JSON j = JSON::parse(body);
	id = j["you"];
	game_id = j["game_id"];
	turn = j["turn"];
	height = j["height"];
	width = j["width"];

	snakes = vector<Snake>();
	for (auto snake : j["snakes"]) {
		Snake s = Snake(snake);
		snakes.push_back(s);
	}

	food = vector<Point>();
	for (auto p : j["food"]) {
		food.push_back(Point(p[0], p[1]).convert());
	}

	updateBoard();
	//board.print();
	getMySnake();
	assert(!snake.id.compare(id));
}

int GameInfo::parseMode(string str) {
	if (!str.compare("classic")) {
		return CLASSIC;
	}
	if (!str.compare("advanced")) {
		return ADVANCED;
	}
	return -1;
}

void GameInfo::updateBoard() {
	board = GameBoard(width, height);
	//TODO fix loop
	int i = 0;
	for (auto snake : snakes) {
		int head = snakes.size();
		for (auto p : snake.coords) {
			board.board[p.y][p.x] = int(i) + head;
		}
		head = 0;
		i++;
	}

	for (auto p : food) {
		board.board[p.y][p.x] = FOOD;
	}
}

void GameInfo::getMySnake() {
	for (auto s : snakes) {
		if (!s.id.compare(id)) {
			snake = s;
			break;
		}
	}
}

Path GameInfo::breadthFirstSearch(Point start, vector<int> targets, bool geq) {
	queue<Point> q = queue<Point>();

	Point parent[board.board.size()][board.board[0].size()];

	Point curpoint = start;
	q.push(curpoint);

	int depth = 0;

	while (!q.empty()) {
		curpoint = q.front();
		q.pop();

		vector<Point> points = curpoint.expand();
		for (auto point : points) {
			if (!board.isVisited(point) && board.isValid(point)) {
				parent[point.y][point.x] = curpoint;
				q.push(point);
				board.markVisited(point);

				for(auto target: targets){
					if ((!geq && (board.getCoord(point) == target)) || (geq && (board.getCoord(point) >= target))) {
						Path path = Path();
						path.target = target;

						int loop = 0;
						//walk path
						while(start.x != point.x || start.y != point.y ){
							path.path.insert(path.path.begin(), point);
							point = parent[point.y][point.x];

							loop++;
							assert(loop < ((height + 2) * (width + 2)));
						}
						path.path.insert(path.path.begin(), start);

						return path;
					}
				}
			}
		
		}

		depth++;
		//Crash if loop
		assert(depth < ((height + 2) * (width + 2)));
	}

	return Path();
}

Path::Path(){
	path = vector<Point>();
	target = EMPTY;
}

int Path::getStepDir(int step){
	assert(step >= 0 && step < path.size() - 1);
	Point one = path[step];
	Point two = path[step + 1]; 

	int x = two.x - one.x;
	int y = two.y - one.y;

	assert(x != 0 || y != 0);

	if(y == -1){
		return NORTH;
	}else if (y == 1){
		return SOUTH;
	}else if (x == 1){
		return EAST;
	}

	return WEST;
}