#include <iostream> 


class Window
{
    public:
        Window(const uint16_t& width, const uint16_t& height, const std::string& windowName);
        bool IsOpen();
        void Destroy();
        void SwapBuffers();

};