#include <common.hpp>
#include "web.hpp"
//https://nxxm.github.io/xxhr/
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "render/renderer.hpp"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

bool shouldRun = true;

SDL_Renderer *renderer;
SDL_Window *window;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool show_demo_window = true;
bool show_another_window = false;

SDL_Texture *renderTexture = NULL;
SDL_Surface* surf = NULL;




#ifdef __EMSCRIPTEN__
//https://emscripten.org/docs/api_reference/emscripten.h.html#calling-javascript-from-c-c

  

EM_BOOL touch_down_callback(int eventType, const EmscriptenTouchEvent *event, void *userData) {
    ImGuiIO &io = ImGui::GetIO();  (void)io;

    //so the good news:
    //this works on chrome ios
    //the bad news:
    //we handle our touch BEFORE imgui / sdl does so io.WantTextInput wont get set until another touch
    //could be worse some people actually add that behaviour intentionally


     if(io.WantTextInput){
        EM_ASM({
            var el = document.getElementById("editor");
            if(el  ) { //!(document.activeElement === el)
               // el.style.display = "block";
                el.focus();
                //el.click();
                //el.style.display = "none";
            }
        });
     }
     else{ 
         EM_ASM({ 
            //make a dummy button to close keyboard
            const dummyInput = document.createElement('input');
            dummyInput.setAttribute('type', 'button'); 
            document.body.appendChild(dummyInput);
            dummyInput.focus();
            document.body.removeChild(dummyInput);
        });
     }

    return 0;
}



#endif



RendererBase* render = nullptr;
void mainloop(void) /* https://wiki.libsdl.org/SDL3/README/emscripten */
{
    if (!shouldRun){
#ifdef __EMSCRIPTEN__
        render->cleanup();
        emscripten_cancel_main_loop(); //it's a website
#else
        exit(0);
#endif
    }
   
       
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            shouldRun = false;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
            shouldRun = false;
    }
    render->render();

}



// Main code
int main(int, char **)
{
    LOGF("%s","====hope====");
    shouldRun = true;
    // Setup SDL
    render = new Renderer();

    if(int err = render->init(); err) {
        ERRORF("Init Renderer Failed [ %d ]", err );
    }
    
    if(is_mobile()){    
        //for virtual keyboard imgui!
        emscripten_set_touchstart_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            NULL,
            1,
            touch_down_callback
        );
    }
    
    LOGF("%s","start main loop!");
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
    


    //should never return!

#else
    while (!shouldRun) mainloop();
#endif

    render->cleanup();
    delete render;

    return 0;
}
