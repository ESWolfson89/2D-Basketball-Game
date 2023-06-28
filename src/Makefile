comp=g++
flags=-std=c++11 -O2
SDLLF=-lSDL2 -lSDL2_image -lSDL2_mixer

launch: main.o game.o graphics.o object.o physics.o player.o point.o
	$(comp) $(flags) main.o game.o graphics.o object.o physics.o player.o point.o $(SDLLF) -o launch
main.o: main.cpp
	$(comp) $(flags) -c main.cpp -o main.o
game.o: game.cpp game.h
	$(comp) $(flags) -c game.cpp -o game.o
graphics.o: graphics.cpp graphics.h globals.h
	$(comp) $(flags) -c graphics.cpp -o graphics.o
object.o: object.cpp object.h globals.h
	$(comp) $(flags) -c object.cpp -o object.o
player.o: player.cpp player.h
	$(comp) $(flags) -c player.cpp -o player.o
physics.o: physics.cpp physics.h
	$(comp) $(flags) -c physics.cpp -o physics.o
point.o: point.cpp point.h
	$(comp) $(flags) -c point.cpp -o point.o

.PHONY: clean

clean:
	rm launch main.o game.o graphics.o object.o physics.o player.o point.o






