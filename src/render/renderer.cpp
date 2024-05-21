#include <common.hpp>
#include <SDL3/SDL_video.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "renderer.hpp"
#include "web.hpp"

#include "../geo.hpp"

#include "GL/gl.h"
#include "globe.hpp"

#include "glm/glm.hpp"


geojson_t worldmap;
geojson_t worldwater;

int Renderer::init(const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0)
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    // Enable native IME.
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");


     Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    window = SDL_CreateWindow("Imgui x SDL3 Renderer x WASM", SCREEN_W, SCREEN_H, window_flags);

    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }

    deduceScreenSize();

    SDL_SetRenderVSync(renderer, 1);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);


    renderer = SDL_CreateRenderer(window, nullptr, 0);

    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.IniFilename = nullptr;                             // we dont want this

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // test emscripten bundling
    ImFont *font = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != nullptr);

    text = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_W, SCREEN_H);
    SDL_assert(text != NULL);



    LOGF("%s","init renderer!");

 

    geo::load(worldmap, "/world/geojson-maps.kyd.au.geo.json");
    geo::load(worldwater, "/world/cities.geojson");
    return 0;
}

void Renderer::cleanup()
{
      // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}




float map_scale = 5.0f;
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};



void Renderer::render()
{

    static Globe globe(w,h);
      //imgui impl of choice 
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    render_overlay();

    bool doin_earth_thing = false;

       // SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    
    if (doin_earth_thing)
    {
        if (SDL_SetRenderTarget(renderer, text) != 0)
        {
            LOGF("wtf %s", SDL_GetError());
        }

        SDL_SetRenderDrawColor(renderer, 0, 235, 255, 255);
        SDL_RenderClear(renderer);
        geo::render(worldmap, renderer, map_scale);
        SDL_SetRenderTarget(renderer, 0);

        globe.Draw(this);
    }
    else{
        SDL_SetRenderDrawColor(renderer, 0,0,10,255);
        SDL_RenderClear(renderer);
        geo::render(worldmap, renderer, map_scale);

    //    SDL_SetRenderDrawColor(renderer, 0,255,255,255);
geo::render(worldwater, renderer, map_scale);
        
    }
    
    
  

    
    //https://github.com/pedro-vicente/render_geojson/blob/master/render_geojson.cc
    
    
  
  
    

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Renderer::deduceScreenSize()
{

    SDL_GetWindowSize(window, &w, &h); //terribly hard deduction
    LOGF("deduced screen size of %i x %i", w, h);
}

void Renderer::render_overlay() //gui
{


    
     ImGuiIO &io = ImGui::GetIO();  (void)io;
    
    static bool show_demo_window = false;
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        

        ImGui::Begin("Map"); // Create a window called "Hello, world!" and append into it.
        static int mobile = 0; //is_mobile();
        ImGui::Text("io.WantTextInput=%d mobile=%d", io.WantTextInput, mobile);          // Display some text (you can use a format strings too)
        ImGui::SameLine();
        ImGui::Checkbox("Demo", &show_demo_window); // Edit bools storing our window open/close state
       

        ImGui::SliderFloat("Map Scale", &map_scale, 0, 50);
        static std::string agent = "dunno";
        if(ImGui::Button("Get UserAgent")){
           
            agent = get_user_agent();
             LOGF("user agent = %s",agent.c_str());
        }

        ImGui::TextWrapped("UserAgent: %s", agent.c_str());
        
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        if(ImGui::CollapsingHeader("Text Input Test")){
             static char text[1024 * 16] =
                "\n"
                " test text can you type on mobile!\n"
                " you can type and edit this\n"
                " crazy huh\n";
               

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
        }
       
        
         


            
         
        ImGui::End();
    }



    // Rendering
   

    //ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}
