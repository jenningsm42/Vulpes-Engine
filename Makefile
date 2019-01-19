LIBS := -lglfw3 -lGLEW -lGL -lX11 -pthread -ldl -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor
CXX := g++
AR := ar
SRC_DIR := ./src
OBJ_DIR := ./obj
OUT_DIR := ./build
OUT_NAME := vulpes.a
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: engine

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $< -o $@

engine: $(OBJ_FILES)
	@echo '** Building engine'
	@mkdir -p $(OUT_DIR)
	$(AR) -cr $(OUT_DIR)/$(OUT_NAME) $^

clean:
	@echo '** Cleaning project'
	$(RM) -r $(OUT_DIR)/* $(OBJ_DIR)/*

.PHONY: all game clean
