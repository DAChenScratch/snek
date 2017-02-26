default:
	g++ -std=c++11 snek.cpp -o snek -w -lpthread -lboost_system 

clean:
	rm ./snek

run:
	./snek 9001 
	
runall:
	./snek 9001 & ./snek 9002 & ./snek 9003 & ./snek 9004 & ./snek 9005 & ./snek 9006 & ./snek 9007 & ./snek 9008 & ./snek 9009 & ./snek 9010 & ./snek 9011 & ./snek 9012


run2: 
	./snek 9001 & ./snek 9002

kill:
	killall snek