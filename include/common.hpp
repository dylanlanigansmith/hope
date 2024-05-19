#pragma once
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif


#include <string>
#include <cstdint>
#include <cassert>
#include <stdio.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

//may as well
#include <SDL3/SDL.h>


#include "log.hpp"

#define SCREEN_W 1280
#define SCREEN_H 720


