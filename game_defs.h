#include <string>
#include <iostream>
#include <vector>
#include "json/src/json.hpp"
#include "crow/amalgamate/crow_all.h"
#include <assert.h>

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

#define EMPTY -1
#define WALL -2
#define FOOD -3 
#define GOLD -4

#define EAT 0
#define ESCAPE 1
#define HUNT 2


class Point {
public:
	int x;
	int y;
	Point(int x1, int y1) {
		x = x1;
		y = y1;
	}
	Point addMove(int move){
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
	Point convert(){
		return Point(x+1, y+1);
	}
};


class GameBoard{
public:
	vector<vector<int>> board;
	GameBoard();
	GameBoard(int width, int height);
	int getCoord(Point p);
	void print();
};


class Snake {
public:
	string id;
	string name;
	int status;
	int age;
	int health;
	vector<Point> coords;
	int kills;
	int food;
	int gold;
	Snake();
	Snake(JSON j);
	int parseStatus(string str); 
};

class GameInfo {
public:
	string id;
	string game;
	int mode;
	int turn;
	int height;
	int width;
	vector<Snake> snakes;
	vector<Point> food;
	vector<Point> walls;
	vector<Point> gold;
	Snake snake;
	GameBoard board;
	GameInfo(string body, string id);
private:
	int parseMode(string str);
	void updateBoard();
	void getMySnake();
};

GameBoard::GameBoard(){
	board = vector<vector<int>>();
}

GameBoard::GameBoard(int width, int height){
	board = vector<vector<int>>();
	for(int i = 0; i < height + 2; i++){
		vector<int> tmp = vector<int>();
		for(int j = 0; j < width + 2; j++){
			if(i == 0 || i == height + 1 || j == 0 || j == width + 1 ){
				tmp.push_back(WALL);
			}else{
				tmp.push_back(EMPTY);
			}
		}
		board.push_back(tmp);
	}
}
int GameBoard::getCoord(Point p){
	assert((p.y >= 0) && (p.y <= board.size()));
	assert((p.x >= 0) && (p.x <= board[p.y].size()));
	return board[p.y][p.x];
}

void GameBoard::print(){
	for(auto v: board){
		for(auto i: v){
			printf("%3d", i);
		}
		cout << endl;
	}
}

Snake::Snake(){
	coords = vector<Point>();
}

Snake::Snake(JSON j){
		id = j["id"];
		name = j["name"];
		status = parseStatus(j["status"]);
		age = j["age"];
		health = j["health"];
		kills = j["kills"];
		food = j["food"];
		gold = j["gold"];

		for (auto p : j["coords"]) {
			coords.push_back(Point(p[0], p[1]).convert());
		}
	}

int Snake::parseStatus(string str) {
	if (!str.compare("alive")) {
		return ALIVE;
	}
	if (!str.compare("dead")) {
		return DEAD;
	}
	return -1;
}

GameInfo::GameInfo(string body, string sid) {
	id = sid;
	JSON j = JSON::parse(body);
	game = j["game"];
	mode = parseMode(j["mode"]);
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

	walls = vector<Point>();
	for (auto p : j["walls"]) {
		walls.push_back(Point(p[0], p[1]).convert());
	}

	gold = vector<Point>();
	for (auto p : j["gold"]) {
		gold.push_back(Point(p[0], p[1]).convert());
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

void GameInfo::updateBoard(){
	board = GameBoard(width, height);
	//TODO fix loop
	int i = 0;
	for(auto snake: snakes){
		for(auto p: snake.coords){
			board.board[p.y][p.x] = int(i);
		}
		i++;
	}

	for(auto p: food){
		board.board[p.y][p.x] = FOOD;
	}

	for(auto p: walls){
		board.board[p.y][p.x] = WALL;
	}

	for(auto p: gold){
		board.board[p.y][p.x] = GOLD;
	}
}

void GameInfo::getMySnake(){
	for(auto s: snakes){
		if(!s.id.compare(id)){
			snake = s;
			break;
		}
	}
}