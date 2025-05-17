
#include "shader.h"
#include "sprite.h"


class Material
{
    private:
        std::weak_ptr<Shader> _shader;
        std::unordered_map<std::string, int32_t> _locationMap;
        std::vector<std::weak_ptr<Sprite>> _sprites;


    public:
        Material(const std::weak_ptr<Shader>& shader) :
            _shader(shader)
        {
            
        }

        int32_t GetLocation(const std::string& key);
        void Bind() const;

        void AddSprite(const std::weak_ptr<Sprite>& sprite)
        {
            _sprites.push_back(sprite);
        }

        void SetVec3(const std::string& name, float x, float y, float z)
        {
            if (const auto shader = _shader.lock())
            {
                const auto location = GetLocation(name);
                shader->SetVec3(location, x, y, z);
            }
        }
};