#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera
{
    private:
        glm::mat4 _projection;

    public:
        Camera() = default;

        Camera(const glm::mat4& projection)
        {
            _projection = projection;
        }

        static Camera Ortho()
        {
            const float orthoPower = 100.0f;
            const float width = 800.0f / orthoPower; // From window
            const float height = 600.0f / orthoPower;// From window

            return Camera(glm::ortho(-width, width , -height, height, 0.1f, 100.0f));
        }

        static Camera Perspective()
        {
            // get from Window: (float)800/(float)600
            return Camera(glm::perspective(glm::radians(45.0f), (float)800/(float)600, 0.1f, 100.0f));
        }

        glm::mat4 GetProjection()
        {
            return _projection;
        }

};