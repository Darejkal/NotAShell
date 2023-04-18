PLUGINS:=$(wildcard ./src/plugins/*.cpp)
FILENAME:=$(patsubst ./src/plugins/%.cpp,./src/plugins/%.exe, $(PLUGINS))
all: plugins main
main: main.cpp
	g++ main.cpp -lgdi32 -o main.exe
	main.exe
plugins: 
	g++ $(PLUGINS) -o $(FILENAME)
