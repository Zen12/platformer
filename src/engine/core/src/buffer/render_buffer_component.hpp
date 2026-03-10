#pragma once
#include <memory>

class RenderBuffer;

struct RenderBufferComponent {
    std::shared_ptr<RenderBuffer> Buffer;
};
