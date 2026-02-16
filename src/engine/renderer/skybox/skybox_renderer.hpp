#pragma once

#include "guid.hpp"

class SkyboxRenderer {
private:
    Guid _materialGuid;
    bool _initialized{false};

public:
    SkyboxRenderer() = default;

    void Initialize(const Guid& materialGuid) {
        _materialGuid = materialGuid;
        _initialized = !materialGuid.IsEmpty();
    }

    [[nodiscard]] bool IsEnabled() const noexcept { return _initialized && !_materialGuid.IsEmpty(); }
    [[nodiscard]] const Guid& GetMaterialGuid() const noexcept { return _materialGuid; }
};
