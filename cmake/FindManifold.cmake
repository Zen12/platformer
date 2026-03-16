include(FetchContent)

if (NOT TARGET manifold)
    set(MANIFOLD_PAR OFF CACHE BOOL "" FORCE)
    set(MANIFOLD_CROSS_SECTION OFF CACHE BOOL "" FORCE)
    set(MANIFOLD_TEST OFF CACHE BOOL "" FORCE)
    set(MANIFOLD_PYBIND OFF CACHE BOOL "" FORCE)
    set(MANIFOLD_CBIND OFF CACHE BOOL "" FORCE)
    set(MANIFOLD_JSBIND OFF CACHE BOOL "" FORCE)
    set(MANIFOLD_EXPORT OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        Manifold
        GIT_REPOSITORY https://github.com/elalish/manifold.git
        GIT_TAG v3.0.1
    )

    FetchContent_MakeAvailable(Manifold)

    # Mark Manifold includes as SYSTEM to suppress -Werror warnings in linalg.h
    get_target_property(_manifold_includes manifold INTERFACE_INCLUDE_DIRECTORIES)
    if(_manifold_includes)
        set_target_properties(manifold PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_manifold_includes}")
    endif()
endif()
