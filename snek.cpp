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


extern string id = "1";

int findFallbackMove(GameInfo game){
	Point head = game.snake.coords[0];
	vector<int> moves = {NORTH, SOUTH, EAST, WEST};
	vector<int> posmoves = vector<int>();
	for(auto m: moves){
		Point p = head.addMove(m);
		int status = game.board.getCoord(p);
		if( status != WALL && status < 0){
			posmoves.push_back(m);
		}
	}

	//prevent %0
	int size = 4;
	if(posmoves.size()){
		size = posmoves.size();
	}

	return posmoves[rand() % size];
}

int decideState(GameInfo game){

}




string moveResponse(int dir) {
	JSON move;
	switch (dir) {
	case NORTH: 
		move["move"] = "north";
		move["taunt"] = "THE NORTH REMEMBERS";
		break;
	case EAST: 
		move["move"] = "east";
		move["taunt"] = "TO THE EAST";
		break;
	case SOUTH: 
		move["move"] = "south";
		move["taunt"] = "SOUTH WHERE ITS WARM";
		break;
	case WEST:
		move["move"] = "west";
		move["taunt"] = "WEST IS BEST";
		break;
	}
	return move.dump();
}

string SnakeInfo() {
	JSON info;
	info["color"] = "#FF0000";
	info["head"] = "";
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
		GameInfo game = GameInfo(req.body, id);
		JSON taunt;
		taunt["taunt"] = "Test Taunt";
		return taunt.dump();
	});

	//MOVE
	CROW_ROUTE(app, "/move")
	.methods("POST"_method)
	([](const crow::request & req) {
		clock_t t = clock();
		GameInfo game = GameInfo(req.body, id);
		int i = findFallbackMove(game);
		t = clock() - t;
		cout << "Exec Move Time: " << ((float) t )/CLOCKS_PER_SEC << endl;
		return moveResponse(i);
	});


	//END
	CROW_ROUTE(app, "/end")
	.methods("POST"_method)
	([](const crow::request & req) {
		GameInfo game = GameInfo(req.body, id);
		return "{}";
	});

	return app;
}

int main(int argc, char **argv)
{	
	int port = 7000;
	if(argc == 3){
		port = atoi(argv[1]);
		id = string(argv[2]);
	}
	SimpleApp app = initSnakeApp();
	app.port(port).multithreaded().run();
}