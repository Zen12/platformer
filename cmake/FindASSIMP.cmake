include(FetchContent)

# Only fetch once
if (NOT TARGET assimp)
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.2.5  # Downgraded for WASM compatibility (v5.4.3 has glTF WASM bug #6077)
    )

    # Assimp options - disable unnecessary features to speed up build
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

    # CRITICAL: Enable glTF 2.0 importer for GLB files
    set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_GLTF_EXPORTER OFF CACHE BOOL "" FORCE)

    # Disable glTF 1.0 importer to fix WASM importer order issue (Issue #6077)
    # The glTF 1.0 importer incorrectly claims .glb files before glTF 2.0 in WASM builds
    add_compile_definitions(ASSIMP_BUILD_NO_GLTF1_IMPORTER)

    FetchContent_MakeAvailable(assimp)

    # Assimp creates its own target, so we don't need to create an alias
    # The target is named 'assimp'

    # Optional: expose include dirs as a variable
    set(ASSIMP_INCLUDE_DIRS ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include CACHE INTERNAL "Assimp include dirs")
endif ()
