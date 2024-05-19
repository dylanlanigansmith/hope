#include "renderer.hpp"
#include "web.hpp"
#include <SDL3/SDL_video.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
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

    text = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    SDL_assert(text != NULL);



    LOGF("%s","init renderer!");
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


uint32_t HSVtoRGB(float h, float s, float v) {
    float r, g, b;

    int i = static_cast<int>(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
    }

    uint8_t R = static_cast<uint8_t>(r * 255);
    uint8_t G = static_cast<uint8_t>(g * 255);
    uint8_t B = static_cast<uint8_t>(b * 255);

    return (R << 24) | (G << 16) | (B << 8) | 0xFF; // Assuming RGBA format
}
void Renderer::render()
{
    render_overlay();

       // SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);

    SDL_LockTextureToSurface(text, NULL, &surf);
    int x,y;
    float hue = 0.0f;
    float hueIncrement = 1.0f / SCREEN_W; 
    for(y = 0; y < SCREEN_H; ++y)
        for(x = 0; x < SCREEN_W; ++x){
            
            int index = (y * surf->pitch / 4) + x;
            ((uint32_t*)surf->pixels  )[index] = HSVtoRGB(hue, 1.0f, 1.0f);
                hue += hueIncrement;
            if (hue > 1.0f) {
                hue -= 1.0f;
            }
        }


    x = SCREEN_W - 1; y = SCREEN_H - 1;
    int index = (y * surf->pitch / 4) + x;
    ((uint32_t*)surf->pixels  )[index] = 0xFF0000FF;
    SDL_UnlockTexture(text);
   

    SDL_RenderTexture(renderer, text, NULL, NULL);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Renderer::deduceScreenSize()
{

    SDL_GetWindowSize(window, &w, &h);
    LOGF("deduced screen size of %i x %i", w, h);
}

void Renderer::render_overlay() //gui
{


      // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    
     ImGuiIO &io = ImGui::GetIO();  (void)io;
    ImGui::NewFrame();
    static bool show_demo_window = true;
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Emscripten"); // Create a window called "Hello, world!" and append into it.
        static int mobile = 0; //is_mobile();
        ImGui::Text("io.WantTextInput=%d mobile=%d", io.WantTextInput, mobile);          // Display some text (you can use a format strings too)

        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
       

        
        static std::string agent = "dunno";
        if(ImGui::Button("Get UserAgent")){
           
            agent = get_user_agent();
             LOGF("user agent = %s",agent.c_str());
        }

        ImGui::TextWrapped("UserAgent: %s", agent.c_str());
        
       
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
          static char text[1024 * 16] =
                "/*\n"
                " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                " the hexadecimal encoding of one offending instruction,\n"
                " more formally, the invalid operand with locked CMPXCHG8B\n"
                " instruction bug, is a design flaw in the majority of\n"
                " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                " processors (all in the P5 microarchitecture).\n"
                "*/\n\n"
                "label:\n"
                "\tlock cmpxchg8b eax\n";

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
         
        ImGui::End();
    }



    // Rendering
    ImGui::Render();

    //ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}
