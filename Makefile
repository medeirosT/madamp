all:
	gcc main.cpp -Wall -ltag_c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer `sdl2-config --cflags --libs` -Os -o madamp
debug:
	gcc main.cpp -Wall -ltag_c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer `sdl2-config --cflags --libs` -g3 -ggdb -o madamp

clean:
	rm madamp
