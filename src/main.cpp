#include <common.hpp>
//https://nxxm.github.io/xxhr/
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"


#include <SDL3/SDL.h>
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
    EM_JS(char*, js_get_user_agent,(),{

        //c str = utf 8 js str = utf 16 !!!
        var useragent = window.navigator.userAgent;
        console.log("get_user_agent()");
        return stringToNewUTF8(useragent);
        //window.navigator.userAgent
    });
#endif

std::string get_user_agent(){
    char* ua = js_get_user_agent();
    auto ret = std::string(ua);
    free(ua);

    return ret;
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


void render_cleanup(){
     // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void mainloop(void) /* https://wiki.libsdl.org/SDL3/README/emscripten */
{
    if (!shouldRun)
    {
#ifdef __EMSCRIPTEN__
        render_cleanup();
        emscripten_cancel_main_loop(); /* this should "kill" the app. */
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

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))
        {
            counter++;
        }

        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        
        static std::string agent = "unknown";
        if(ImGui::Button("Get UserAgent")){
            agent = get_user_agent();
        }

        ImGui::TextWrapped("UserAgent: %s", agent.c_str());
        
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }



    // Rendering
    ImGui::Render();
    // SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(renderer);

    SDL_LockTextureToSurface(renderTexture, NULL, &surf);
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
    SDL_UnlockTexture(renderTexture);
   

    SDL_RenderTexture(renderer, renderTexture, NULL, NULL);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

// Main code
int main(int, char **)
{
    shouldRun = true;
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0)
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Enable native IME.
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with SDL_Renderer graphics context
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    window = SDL_CreateWindow("Imgui x SDL3 Renderer x WASM", SCREEN_W, SCREEN_H, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    renderer = SDL_CreateRenderer(window, nullptr, 0);

    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetRenderVSync(renderer, 1);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
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



    renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    SDL_assert(renderTexture != NULL);

// Main loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    while (!shouldRun)
    {
        mainloop();
    }
#endif

    render_cleanup();

    return 0;
}
