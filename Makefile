default:
	g++ -std=c++11 snek.cpp -o snek -w -lpthread -lboost_system 

clean:
	rm ./snek

run:
	./snek 9001 
	
runall:
	./snek 9001 1 & ./snek 9002 2 & ./snek 9003 3 & ./snek 9004 4 & ./snek 9005 5 & ./snek 9006 6 & ./snek 9007 7 & ./snek 9008 8 & ./snek 9009 9 & ./snek 9010 10 & ./snek 9011 11 & ./snek 9012 12 


run2: 
	./snek 9001 & ./snek 9002

kill:
	killall snek