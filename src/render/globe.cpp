#include "globe.hpp"
#include "renderer.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"


SDL_Texture* make_test_texture(SDL_Renderer* render)
{
    

    SDL_Texture* text = NULL;
    SDL_Surface* surf = NULL;


    text = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
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

    return text;
}

void Globe::Draw(RendererBase *renderer)
{
    if(screen_vertices){

        auto render = (Renderer*)(renderer);
        if(!texture){
            texture = make_test_texture(render->get());
        }

        SDL_RenderGeometry(render->get(), render->get_text(), screen_vertices, vertices.size(), indices.data(), indices.size());
   }

   ImGui::Begin("Globe");

    ImGui::SeparatorText("Lighting");

    static glm::vec3 lightdir = {1,1,1};
    if(ImGui::SliderFloat3("light pos", (float*)&lightdir, -5.f,5.f)){
        light.dir_normalized = glm::normalize(lightdir);
        UpdateScreenVertices(1);
    }
    ImGui::SeparatorText("Location");
   static glm::vec3 axis = {1.f, 1.f, 1.f};
    float angle = 0.f;
    ImGui::InputFloat3("axis", (float*)&axis);
    
    if(ImGui::SliderFloat("angle (deg)", &angle, -360.f, 360.f)){
        float angle_rad = DEG2RAD(angle);
        mat.rotate(angle_rad, axis);
        UpdateScreenVertices();
    }


    if(ImGui::SliderFloat2("offset", (float*)&offset, 0.f, 2500.f, ".0f")){
        offset.x = (int)offset.x;
        offset.y = (int)offset.y;
        UpdateScreenVertices();
    }

    static glm::vec3 look = glm::vec3(0.0f, 0.0f, 5.0f);

    if(ImGui::SliderFloat3("lookat", (float*)&look, -5.f,5.f)){
        mat.view = glm::lookAt(look, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
        mat.update();
        UpdateScreenVertices();
        
    }


    if(ImGui::Button("UpdateVerts")){
        UpdateScreenVertices();
    } ImGui::SameLine();
     if(ImGui::Button("UpdateFull")){
        GenSphere3D();
        UpdateScreenVertices();
    }
   ImGui::End();

}

void Globe::Create()
{
   screen_vertices = nullptr;

    light.dir_normalized = glm::normalize(glm::vec3(-1.0f, 1.0f, 0.50f));
    light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light.update = false;


   

    mat.proj = glm::perspective(glm::radians(45.0f), screen.z, 0.1f, 100.0f);
    mat.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
    mat.model = glm::mat4(1.0f);
    mat.mvp = mat.update();
    GenSphere3D();

    UpdateScreenVertices();


    
}

void Globe::GenSphere3D()
{
    vertices.clear();
    indices.clear();
    
    // Generates 3D Sphere Coords
    // maybe? this would be easy if i wasn't using sdl3 renderer for fun

     float x, y, z, xy;                           // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
    float s, t;                                  // vertex texCoord

    float sectorStep = 2 * M_PI / sectors; // hoz slice
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i)
    {
        stackAngle = (M_PI / 2) - (i * stackStep); // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);            // r * cos(u)
        z = radius * sinf(stackAngle);             // r * sin(u)

        for (int j = 0; j <= sectors; ++j)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            vertices.push_back({glm::vec3(x, y, z),
                                glm::vec3(x * lengthInv, y * lengthInv, z * lengthInv),
                                glm::vec2((float)j / sectors, (float)i / stacks)});

            // indices
            if (i != 0 && j != 0)
            {
                int current = i * (sectors + 1) + j;
                int previous = current - (sectors + 1);

                indices.push_back(previous);
                indices.push_back(current);
                indices.push_back(previous - 1);

                indices.push_back(previous - 1);
                indices.push_back(current);
                indices.push_back(current - 1);
            }
        }
    }
}

void Globe::UpdateScreenVertices(int full, int shading_type)
{
        
    
        glm::vec3 lightDir = light.dir_normalized ; 
        glm::vec3 ambientColor = glm::vec3(0.2f); // Adjust the intensity as needed

    if(!screen_vertices)
        screen_vertices = new SDL_Vertex[vertices.size()];

    mat.update();


    for (size_t i = 0; i < vertices.size(); ++i)
    {
       if(!full) //update screen verts, base 3d geo changed
       {
             glm::vec4 pos = mat.mvp * glm::vec4(vertices[i].pos, 1.0f);
        // pos /= pos.w;  // Perform perspective divide

        // Convert to screen space
       // screen_vertices[i].position.x = ((pos.x * 0.5f + 0.5f));
      //  screen_vertices[i].position.y = ((pos.y * 0.5f + 0.5f));

        screen_vertices[i].position = mat.to_screen({pos.x, pos.y, pos.z});

        screen_vertices[i].position.x += offset.x;
        screen_vertices[i].position.y += offset.y;

        // screen_vertices[i].color = {0,0,0,0};
        screen_vertices[i].tex_coord = {vertices[i].tex.x, vertices[i].tex.y};
       }
       

        

        if (shading_type == 0)
        {
            screen_vertices[i].color = {0.f, 0.9f, 1.f, 1.f};
            continue;
        } else if (shading_type < 0){ //< 0
            screen_vertices[i].color = {1.f, 1.f, 1.f, 1.f};
            continue;
        }
        else{
            float diffuseIntensity = glm::max(glm::dot(vertices[i].normal, lightDir), 0.0f);
            glm::vec3 ambientIntensity = ambientColor;

            // Combine ambient and diffuse intensities
            glm::vec3 totalIntensity = ambientIntensity + diffuseIntensity;

            // Ensure the color components are clamped between 0 and 1
            glm::vec3 finalColor = glm::clamp(totalIntensity, 0.0f, 1.0f);
            finalColor *= 0.75;

            // Set vertex color based on shading
            screen_vertices[i].color = {finalColor.x, finalColor.y, finalColor.z, 1.f};
        }
        
    }
}
