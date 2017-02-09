#include "crow/amalgamate/crow_all.h"
#include <iostream>

using namespace std;
using namespace crow;



int main()
{
    SimpleApp app;

    //INFO
    CROW_ROUTE(app, "/")([](){
    	crow::json::wvalue info;
    	info["color"] = "#FF0000";
    	info["head"] = "";
    	return info; 
    });

    //START
    CROW_ROUTE(app, "/start")
    .methods("POST"_method)
    ([](const crow::request& req){
    	cout << "START" << endl;
        return "START";
    });

    //MOVE
    CROW_ROUTE(app, "/move")
    .methods("POST"_method)
    ([](const crow::request& req){
    	cout << "MOVE" << endl;
        return "MOVE";
    });


    //END
    CROW_ROUTE(app, "/end")
    .methods("POST"_method)
    ([](const crow::request& req){
    	cout << "END" << endl;
        return "END";
    });

    app.port(7000).multithreaded().run();
}