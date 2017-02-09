default:
	g++ -std=c++11 snek.cpp crow_all.h -o snek -w -lpthread -lboost_system 


clean:
	rm ./snek
