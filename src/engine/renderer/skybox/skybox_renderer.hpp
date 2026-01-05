#pragma once

#include <string>

class SkyboxRenderer {
private:
    std::string _materialGuid;
    bool _initialized{false};

public:
    SkyboxRenderer() = default;

    void Initialize(const std::string& materialGuid) {
        _materialGuid = materialGuid;
        _initialized = !materialGuid.empty();
    }

    [[nodiscard]] bool IsEnabled() const noexcept { return _initialized && !_materialGuid.empty(); }
    [[nodiscard]] const std::string& GetMaterialGuid() const noexcept { return _materialGuid; }
};
