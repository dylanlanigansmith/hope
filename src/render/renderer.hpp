#pragma once
#include "rendererbase.hpp"

class Renderer : public RendererBase {
public:
    Renderer() : renderer(0), text(0), surf(0)  {}
    virtual ~Renderer() {}

    virtual int init(const char* title = "SDL3 x IMGUI x WASM");
    virtual void cleanup();
    virtual void render();


    inline auto& get() { return renderer; }


protected:

    void deduceScreenSize();

    void render_overlay();


private:    
    
    SDL_Renderer *renderer;

    SDL_Texture* text;

    SDL_Surface* surf;

   
};