SFML_PATH = /opt/homebrew/Cellar/sfml/3.0.2
GLM_PATH = external

cppFileNames := $(shell find ./src -type f -name "*.cpp")

all: compile

compile:
	mkdir -p bin
	clang++ $(cppFileNames) -std=c++17 -g -I$(SFML_PATH)/include -I$(GLM_PATH) -o bin/app -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network