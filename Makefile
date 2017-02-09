default:
	g++ -std=c++11 snek.cpp -o snek -w -lpthread -lboost_system 


clean:
	rm ./snek
