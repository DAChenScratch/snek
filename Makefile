default:
	g++ -std=c++11 snek.cpp -o snek -w -lpthread -lboost_system 

clean:
	rm ./snek

run: 
	./snek 7000 1 & ./snek 8000 2 & ./snek 9000 3

kill:
	killall snek