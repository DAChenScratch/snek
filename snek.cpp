#include "crow/amalgamate/crow_all.h"
#include "json/src/json.hpp"
#include "game_defs.h"
#include <iostream>
#include <string>
#include <vector>


using namespace std;
using namespace crow;
using JSON = nlohmann::json;


string SnakeInfo() {
	JSON info;
	info["color"] = "#FF0000";
	info["head"] = "";
	return info.dump();
}

SimpleApp initSnakeApp(){
	SimpleApp app;

	//INFO
	CROW_ROUTE(app, "/")([]() {
		return SnakeInfo();
	});

	//START
	CROW_ROUTE(app, "/start")
	.methods("POST"_method)
	([](const crow::request & req) {
		return "START";
	});

	//MOVE
	CROW_ROUTE(app, "/move")
	.methods("POST"_method)
	([](const crow::request & req) {
		GameInfo game = GameInfo(req.body);
		return "MOVE";
	});


	//END
	CROW_ROUTE(app, "/end")
	.methods("POST"_method)
	([](const crow::request & req) {
		return "END";
	});

	return app;
}

int main()
{	
	SimpleApp app = initSnakeApp();
	app.port(7000).multithreaded().run();
}