default:
	g++ -std=c++11 snek.cpp -o snek -w -lpthread -lboost_system 

clean:
	rm ./snek

run: 
	./snek 7000 & ./snek 8000 & ./snek 9000

kill:
	killall snek