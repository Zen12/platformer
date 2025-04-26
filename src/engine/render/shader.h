#include <string>
#include <iostream>


class Shader
{
private:
    uint32_t shaderProgram;
    uint32_t fragmentShader;
    uint32_t vertexShader;
public:
    Shader() = default;
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    ~Shader();

    void Use() const;
    uint32_t GetShaderId() const
    {
        return shaderProgram;
    }
};

