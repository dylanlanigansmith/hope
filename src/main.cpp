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

float add_to_mapscale = 0.f;
std::string event_str = "no events";
RendererBase* render = nullptr;


size_t last_num_fingers = 0;
std::pair<SDL_TouchFingerEvent, SDL_TouchFingerEvent> fl =  {{.fingerID = 0xffff},{.fingerID = 0xffff}};





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
    
    std::unordered_map<uint64_t, std::vector<SDL_TouchFingerEvent>> fing = {};
    event_str = "no events";
    while (SDL_PollEvent(&event))
    {
         ImGuiIO &io = ImGui::GetIO();  (void)io;
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            shouldRun = false;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
            shouldRun = false;

        if(event.type == SDL_EVENT_FINGER_MOTION && !io.WantCaptureMouse){
            //LOGF("touch: %lu", event.tfinger.fingerID);
            fing[event.tfinger.fingerID].push_back(event.tfinger);

        }
      //  if(event.type == SDL_EVENT_GESTURE //https://github.com/libsdl-org/SDL/pull/9445/commits/023cf243819a30ddcacb7cdf78a20e5cee9c495e
       
    }
    
    //do we really have to do the whole track this touch, is zoom thing? :9
     //https://developer.mozilla.org/en-US/docs/Web/API/Pointer_events/Pinch_zoom_gestures
    if(fing.size() == 2 ){ 
        //we are double touching
         //   EM_ASM("alert(\"double touch?\")"); //this works

        event_str = std::string(std::to_string(fing.size())); 
        if(last_num_fingers == fing.size()){
            
            
            //find touch 
            if(auto id = fing.begin()->first; (id == fl.first.touchID || id == fl.second.touchID) ) { //new touch finger same as last
            
                if(id = fing.end()->first; (id == fl.first.touchID || id == fl.second.touchID) ){
                    //we know our two last fings = two cur fings

                 //   event_str += std::string(" two fingers ") + std::to_string(fing.begin()->first) + std::string(" / ") + std::to_string(fing.end()->first);
                    float old_dist = sqrtf( pow(fl.second.x -  fl.first.x, 2) + pow(fl.second.y  - fl.first.y, 2)   );

                    event_str = "ZZ";
                    float new_dist = sqrtf( pow(fing.begin()->second.back().x -  fing.end()->second.back().x, 2) + pow(fing.begin()->second.back().y - fing.end()->second.back().y, 2)   );

                   if(new_dist > old_dist){ //zoom in
                        event_str += "zoom in ";
                       //  event_str += std::to_string(add_to_mapscale);
                        add_to_mapscale += 1;
                   }
                   else{ //zoom out
                        event_str += "zoom out ";
                      //  event_str += std::to_string(add_to_mapscale);
                        add_to_mapscale -= 1;
                   }
                }
            }

         

             // //  if(id == fl.first.touchID){
             //       last = &fl.first;
             //   }
             //   else if( id == fl.second.touchID) {
              //      last = &fl.second;
              // } else{
                    //cant handle case in loop
        
        }
           fl.first = fing.begin()->second[0];
            fl.second = fing.end()->second[0];
    } else{
        fl.first.touchID = 0xffff; fl.second.touchID = 0xffff; //invalidate
    }

    last_num_fingers = fing.size();

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
