#pragma once
#include <common.hpp>
#include "renderertypes.hpp"
class RendererBase
{
public:
    RendererBase() : w(0), h(0), window(NULL) { init_platform(); }
    virtual ~RendererBase() {}

    virtual int init(const char* title = "HOPE") = 0;
    virtual void cleanup() = 0;
    virtual void render() = 0;

  

    inline auto width() { return w; }
    inline auto height() { return h; }


    inline auto& sdl_window() { return window; }

    bool mobile();
private:
    void init_platform();


protected:
    int w, h;
    SDL_Window* window;
    
    bool plat_mobile;

};