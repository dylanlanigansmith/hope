#include <common.hpp>
#include <SDL3/SDL_video.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rendererbase.hpp"

class Globe
{
public:
    Globe(float fw, float fh, float radius = 100.f, glm::vec2 off = {200.f, 200.f}, int sectors = 36, int stacks = 18) 
        : radius(radius), offset(off), sectors(sectors), stacks(stacks) { 
            float aspect_ratio = 1.f; //yeah
            screen = glm::vec3(fw, fh, aspect_ratio); 

            this->Create(); 
        }
    ~Globe() { 
        if(screen_vertices) delete[] screen_vertices; 
        if(texture) SDL_free(texture);
    }


    void Draw(RendererBase* renderer);

    
    
protected:
    void Create();

    void GenSphere3D();

    void UpdateScreenVertices(int full = 0, int shading_type = 1);
private:
    
    float radius;
    glm::vec2 offset;
    int sectors;
    int stacks;

    std::vector<vertex_t> vertices;
    std::vector<int> indices;
    SDL_Vertex* screen_vertices = NULL;
    glm::vec3 screen;

    struct {
        glm::vec3 ambient;
        glm::vec3 dir_normalized;

        bool update = false;
    } light;

    
    mat3d_t mat; 

    SDL_Texture* texture = NULL; 
};