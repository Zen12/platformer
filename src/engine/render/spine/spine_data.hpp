#pragma once

#include "spine_mesh.hpp"
#include "spine/Atlas.h"
#include "spine/Skeleton.h"

class SpineData
{
private:
    std::unique_ptr<spine::Atlas> _atlas;
    std::unique_ptr<spine::Skeleton> _skeleton;
public:
    SpineData(spine::Atlas* atlas,
        spine::Skeleton* skeleton):
        _atlas(atlas),
        _skeleton(skeleton)
    {
    }

    void Bind() const {
        //_mesh->Bind();
    }

    [[nodiscard]] size_t GetIndicesCount() const {
        return 0;
        //return _mesh->GetIndicesCount();
    }
};
