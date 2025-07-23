SDL3_FLAGS := `pkg-config --cflags --libs sdl3`

build:
	g++ -std=c++11 -Wall main.cpp -o animations {{SDL3_FLAGS}} -lm

release:
	g++ -O3 -std=c++11 -Wall main.cpp -o animations {{SDL3_FLAGS}} -lm

debug:
	g++ -g -std=c++11 -Wall main.cpp -o animations {{SDL3_FLAGS}} -lm

run: build
	./animations

clean:
	rm -f animations
