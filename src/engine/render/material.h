
#include "shader.h"
#include "sprite.h"
#include "../asset/asset_loader.h"


class Material
{
    private:
        std::unique_ptr<Shader> _shader;
        std::unordered_map<std::string, int32_t> _locationMap;
        std::vector<std::weak_ptr<Sprite>> _sprites;


    public:

        Material() = delete;

        Material(const std::string& vertexSource, const std::string& fragmentSource)
        {
            _shader = std::make_unique<Shader>(AssetLoader::LoadShaderFromPath(vertexSource, fragmentSource));
        }

        int32_t GetLocation(const std::string& key);
        void Bind() const;

        void AddSprite(const std::weak_ptr<Sprite>& sprite)
        {
            _sprites.push_back(sprite);
        }

        uint32_t GetShaderId() const
        {
            return _shader->GetShaderId();
        }

        void SetVec3(const std::string& name, float x, float y, float z)
        {
            const auto location = _shader->GetLocation(name);
            _shader->SetVec3(location, x, y, z);
        }
};