
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
EM_LDFLAGS:=-sUSE_WEBGL2=1  --shell-file $(EM_SHELLFILE)
LDFLAGS=$(EM_LDFLAGS)

INCLUDES:=-I$(SDL_INC)/ -I./include/ -I$(IMGUI_DIR)/
LIBS:=-L$(SDL_DIR)/build -lSDL3


OUTFILE:=main
OUTDIR:=./build
OUTEXT:=html

OUT:=$(OUTDIR)/$(OUTFILE).$(OUTEXT)
$(info Final Product: '$(OUT)')
#Emscripten Preload
#https://emscripten.org/docs/porting/files/packaging_files.html
RESOURCE_DIR:=./resource

USE_FILE_SYSTEM ?= 1
ifeq ($(USE_FILE_SYSTEM), 0)
LDFLAGS += -s NO_FILESYSTEM=1
CFLAGS += -DIMGUI_DISABLE_FILE_FUNCTIONS
endif
ifeq ($(USE_FILE_SYSTEM), 1)
LDFLAGS += --no-heap-copy --preload-file $(RESOURCE_DIR)@/
endif

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
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJ) $(LIBS) -o $(OUT)

%.o : %.cpp $(HEADERS)
	@echo "[ $< ]"
	$(CXX) $(CFLAGS) $(INCLUDES)  -c $< -o $@

.PHONY: run
run: clean all
	emrun $(OUT)
#-sNO_FILESYSTEM=1 -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1 



.PHONY: clean_deps
clean_deps:
	rm -rf $(SDL_LIB)
	rm -rf $(IM_OBJ)

.PHONY: clean
clean:
	rm -rf $(OUTDIR)/$(OUTFILE).*
	find ./src/ -type f -name '*.o' -delete