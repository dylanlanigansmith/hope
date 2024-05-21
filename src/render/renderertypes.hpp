#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define M_RADPI		57.295779513082f
#define M_PI_F		((float)(M_PI))	
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )


struct vertex_t {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 tex;
};


struct mat3d_t{
    glm::mat4 proj;
    glm::mat4 view ;
    glm::mat4 model;;
    glm::mat4 mvp;

    inline void rotate(float rad, const glm::vec3& axis){
        model = glm::rotate(glm::mat4(1.0f), rad, axis);
    }

    inline auto& update(){
        mvp = proj * view * model; return mvp;
    }

    inline SDL_FPoint to_screen(glm::vec3 world){
        glm::vec4 pos = mvp * glm::vec4(world, 1.0f);
     //   return SDL_FPoint {(pos.x * 0.5f + 0.5f), (pos.y * 0.5f + 0.5f) };
      return SDL_FPoint {(pos.x ), (pos.y ) };
    }
};

//junk drawer
static inline uint32_t HSVtoRGB(float h, float s, float v) {
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


