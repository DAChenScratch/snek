#include "crow/amalgamate/crow_all.h"
#include "json/src/json.hpp"
#include "game_defs.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <time.h>




using namespace std;
using namespace crow;
using JSON = nlohmann::json;

int findFallbackMove(GameInfo game) {
	Point head = game.snake.getHead();
	vector<int> posmoves = vector<int>();
	for (auto m : moveslist) {
		Point p = head.addMove(m);
		if (game.board.isValid(p)) {
			posmoves.push_back(m);
		}
	}

	//dead end
	if (!posmoves.size()) {
		return 0;
	}

	return posmoves[rand() % posmoves.size()];
}

int eat(GameInfo game) {
	Point head = game.snake.getHead();
	Path path = game.breadthFirstSearch(head, {FOOD}, false);

	// found path
	if (path.path.size() > 1){
		return path.getStepDir(0);
	}

	return findFallbackMove(game);
}

int orbit(GameInfo game){
	Point head = game.snake.getHead();
	Point tail = game.snake.getTail();

	Path path = game.astarGraphSearch(head, tail);


	return findFallbackMove(game);
}

int executeState(GameInfo game, int state) {
	switch (state) {
	case EAT:
		return eat(game);
		break;
	case FINDFOOD:
		return findFallbackMove(game);
		break;

	case ORBIT:
		return orbit(game);
		break;

	case DEFAULT:
		return findFallbackMove(game);
		break;
	}
	return findFallbackMove(game);
}

int decideState(GameInfo game) {
	if (game.snake.health < 30) {
		return EAT;
	}
	if (game.snake.health < 100) {
		return ORBIT;
	}

	if (game.snake.health < 100) {
		return FINDFOOD;
	}
	return DEFAULT;
}


string moveResponse(int dir) {
	JSON move;
	switch (dir) {
	case NORTH:
		move["move"] = "up";
		move["taunt"] = "THE NORTH REMEMBERS";
		break;
	case EAST:
		move["move"] = "right";
		move["taunt"] = "TO THE EAST";
		break;
	case SOUTH:
		move["move"] = "down";
		move["taunt"] = "SOUTH WHERE ITS WARM";
		break;
	case WEST:
		move["move"] = "left";
		move["taunt"] = "WEST IS BEST";
		break;
	}
	return move.dump();
}

string SnakeInfo() {
	JSON info;
	info["color"] = "#FF0000";
	info["head_url"] = "http://www.matagot.com/IMG/cache-20x20/arton27-20x20.jpg";
	info["taunt"] = "Test Taunt";
	info["name"] = "name";
	return info.dump();
}

SimpleApp initSnakeApp() {
	SimpleApp app;

	//INFO
	CROW_ROUTE(app, "/")([]() {
		return SnakeInfo();
	});


	//START
	CROW_ROUTE(app, "/start")
	.methods("POST"_method)
	([](const crow::request & req) {
		return SnakeInfo();
	});

	//MOVE
	CROW_ROUTE(app, "/move")
	.methods("POST"_method)
	([](const crow::request & req) {
		clock_t t = clock();
		GameInfo game = GameInfo(req.body);
		int state = decideState(game);
		int move =  executeState(game, state);
		t = clock() - t;
		cout << "Exec Move Time: " << ((float) t ) / CLOCKS_PER_SEC << endl; 
		cout << moveResponse(move);
		return moveResponse(move);
	});

	return app;
}

int main(int argc, char **argv)
{
	int port = 7000;
	if (argc == 2) {
		port = atoi(argv[1]);
	}
	SimpleApp app = initSnakeApp();
	app.port(port).multithreaded().run();
}