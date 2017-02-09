#include "crow/amalgamate/crow_all.h"
#include "json/src/json.hpp"
#include "game_defs.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>


using namespace std;
using namespace crow;
using JSON = nlohmann::json;


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
	case 
		WEST: move["move"] = "west";
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
		JSON taunt;
		taunt["taunt"] = "Test Taunt";
		return taunt.dump();
	});

	//MOVE
	CROW_ROUTE(app, "/move")
	.methods("POST"_method)
	([](const crow::request & req) {
		GameInfo game = GameInfo(req.body);
		int i = rand() % 4;
		return moveResponse(i);
	});


	//END
	CROW_ROUTE(app, "/end")
	.methods("POST"_method)
	([](const crow::request & req) {
		return "{}";
	});

	return app;
}

int main(int argc, char **argv)
{	
	int port = 7000;
	if(argc == 2){
		port = atoi(argv[1]);
	}
	SimpleApp app = initSnakeApp();
	app.port(port).multithreaded().run();
}