
#locations of deps etc
SDL_DIR:=./SDL
SDL_BUILDDIR:=$(SDL_DIR)/build/
SDL_LIB:=$(SDL_DIR)/build/libSDL3.a
SDL_INC:=$(SDL_DIR)/include

IMGUI_DIR:=./imgui




CCMAKE:=emcmake cmake 

MAKE:=emmake make
MAKE_FLAGS:=-j14


SOURCES = $(wildcard src/**.cpp)
OBJ = ${SOURCES:.cpp=.o}
HEADERS = $(wildcard include/**.hpp)
CFLAGS:=-std=c++17 -lGL

EM_SHELLFILE:=src/shell.html
EM_LINKERFLAGS:=-sUSE_WEBGL2=1  --shell-file $(EM_SHELLFILE)


INCLUDES:=-I$(SDL_INC)/ -I./include/ -I$(IMGUI_DIR)/
LIBS:=-L$(SDL_DIR)/build -lSDL3
OUTFILE:=main

#
# Targets for Dependencies
#

$(SDL_LIB):
	$(CCMAKE) -S $(SDL_DIR) -B $(SDL_BUILDDIR)
	$(MAKE) -C $(SDL_BUILDDIR) $(MAKE_FLAGS) all


IM_SRC = $(wildcard $(IMGUI_DIR)/imgui**.cpp)
IM_OBJ = ${IM_SRC:.cpp=.o}
OBJ += $(IM_OBJ)
imgui/imgui.o: $(SDL_LIB)
	$(CXX) $(CFLAGS) -I$(SDL_INC)/ -c $(IM_SRC) 
#this is greasy
	mv imgui_*.o imgui/
	mv imgui.o imgui/



#Main Targets


.PHONY: all
all: clean main

main: $(OBJ) imgui/imgui.o
	@echo $(OBJ)
	@echo $(IM_OBJ)
	@echo $(IM_SRC)
	$(CXX) $(CFLAGS) $(EM_LINKERFLAGS) $(OBJ) $(LIBS) -o $(OUTFILE).html

%.o : %.cpp $(HEADERS)
	@echo "[ $< ]"
	$(CXX) $(CFLAGS) $(INCLUDES)  -c $< -o $@

.PHONY: run
run: clean all
	emrun $(OUTFILE).html
#-sNO_FILESYSTEM=1 -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1 



.PHONY: clean_deps
clean_deps:
	rm -rf $(SDL_LIB)
	rm -rf $(IM_OBJ)

.PHONY: clean
clean:
	rm -rf $(OUTFILE).*
	find ./src/ -type f -name '*.o' -delete