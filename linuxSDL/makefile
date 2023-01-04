.ONESHELL:

# default
SDL_CONFIG=sdl2-config
CC=g++ -o app

linux: build run

build:
	$(CC) -Wall app.cpp -fopenmp -Wno-narrowing -I$(ZIC_LIB) -I$(ZIC_APP_CORE_LIB) \
	`$(SDL_CONFIG) --cflags --libs`

run:
	echo start app
	./app
