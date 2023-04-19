PLUGINS:=$(wildcard ./src/plugins/*.cpp)
FILENAME:=$(patsubst ./src/plugins/%.cpp,./src/plugins/%.exe, $(PLUGINS))
all: plugins myShell
myShell: myShell.cpp
	g++ myShell.cpp -lgdi32 -o myShell.exe
	myShell.exe
plugins: 
	g++ $(PLUGINS) -o $(FILENAME)
