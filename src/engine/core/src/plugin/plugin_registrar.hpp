#pragma once
#include "plugin_registry.hpp"

template<typename T>
struct PluginRegistrar {
    PluginRegistrar() {
        PluginRegistry::Instance().AddCallback([](PluginRegistries& registries) {
            T::Register(registries);
        });
    }
};

#define REGISTER_PLUGIN(ClassName) \
    static PluginRegistrar<ClassName> s_##ClassName##_registrar;
