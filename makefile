PLUGINS:=$(wildcard ./src/plugins/*.cpp)
# FILENAME:=$(patsubst ./src/plugins/%.cpp,./src/plugins/%.exe, $(PLUGINS))
all: plugins myShell
myShell: myShell.cpp
	g++ myShell.cpp -lgdi32 -lshlwapi -o myShell.exe
	myShell.exe
plugins: $(PLUGINS)
	$(foreach file, $(PLUGINS), g++ $(file) -municode -lpsapi -o $(patsubst ./src/plugins/%.cpp,./src/plugins/%.exe, $(file)) &)