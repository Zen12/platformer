#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>         
#include <glm/gtx/quaternion.hpp>         
#include <glm/gtc/type_ptr.hpp>

#include <map>

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    uint32_t TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    uint32_t Advance;   // Horizontal offset to advance to next glyph
};


class Font
{
    public:
        std::map<char32_t, Character> Characters;
};