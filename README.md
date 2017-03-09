# C++ Snek
Blogpost: https://rdbrck.com/2017/03/slithering-success-battlesnake-2017/

##Dependencies##
Boost C++ http://www.boost.org/

Crow https://github.com/ipkn/crow

JSON for Modern C++ https://github.com/nlohmann/json

##Compiling##
`git submodule init`

`git submodule sync`

`git submodule update`

`make`

##Strategy##
I chose to use C++ so that I could get the most out of the 200ms execution time. For the web server I selected Crow, a C++ web microframework that was inspired by Flask. Lightweight, fast, and simple to use, it was perfect for this project. For JSON parsing I went with JSON for Modern C++ which has a very familiar syntax.

The strategy was very simple. At every turn, a flood fill algorithm was used to find dead ends and fill them. BFS was used to find the nearest food. In the early game, it would eat the closest food until reaching a size of 10. Then, a secondary strategy would kick in. If it was far away from food, it would BFS to the nearest food. Once close, it would A* back to two spaces from its tail, delaying until its health was low. When its health is less than the size path to the nearest food added with a buffer value, it would execute the path found by the BFS to eat the food. This strategy had a major drawback – enemy snakes could steal food from it while it was low on health, causing it to starve.

The snake came 3rd in its first battle, dying to a flood fill bug which did not allow it to grab the food it needed to survive. In its next battle, it lost a one on one to the eventual winner aptly named Better than Aleksiy’s Snake.

Next year’s snake will be improved by rewriting dead end detection, eating food that other snakes are trying to steal, and implementing alpha-beta/minimax to fully utilize the performance of C++.
