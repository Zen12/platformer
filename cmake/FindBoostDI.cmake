include(FetchContent)

# Only fetch once
if (NOT TARGET Boost::DI)
    # Set policy to allow FetchContent_Populate
    cmake_policy(SET CMP0169 OLD)

    FetchContent_Declare(
        boost_di
        GIT_REPOSITORY https://github.com/boost-ext/di.git
        GIT_TAG v1.3.0
    )

    FetchContent_GetProperties(boost_di)
    if(NOT boost_di_POPULATED)
        FetchContent_Populate(boost_di)
    endif()

    # Boost.DI is header-only, create interface library
    add_library(Boost_DI INTERFACE)
    target_include_directories(Boost_DI INTERFACE ${boost_di_SOURCE_DIR}/include)

    # Create alias target
    add_library(Boost::DI ALIAS Boost_DI)

    # Optional: expose include dirs as a variable
    set(BOOST_DI_INCLUDE_DIRS ${boost_di_SOURCE_DIR}/include CACHE INTERNAL "Boost.DI include dirs")
endif ()