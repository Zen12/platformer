include(FetchContent)

# For macOS, we'll download prebuilt FFmpeg libraries
# For a production build, you might want to use system libraries or vcpkg

if(APPLE)
    # Download FFmpeg prebuilt for macOS
    FetchContent_Declare(
        ffmpeg
        URL https://evermeet.cx/ffmpeg/ffmpeg-6.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(ffmpeg)

    # Set FFmpeg paths
    set(FFMPEG_ROOT ${ffmpeg_SOURCE_DIR})

    # For macOS, we'll use homebrew ffmpeg libraries
    find_path(FFMPEG_INCLUDE_DIR
        NAMES libavcodec/avcodec.h
        PATHS
            /opt/homebrew/include
            /usr/local/include
            ${FFMPEG_ROOT}/include
    )

    find_library(AVCODEC_LIBRARY
        NAMES avcodec
        PATHS
            /opt/homebrew/lib
            /usr/local/lib
            ${FFMPEG_ROOT}/lib
    )

    find_library(AVFORMAT_LIBRARY
        NAMES avformat
        PATHS
            /opt/homebrew/lib
            /usr/local/lib
            ${FFMPEG_ROOT}/lib
    )

    find_library(AVUTIL_LIBRARY
        NAMES avutil
        PATHS
            /opt/homebrew/lib
            /usr/local/lib
            ${FFMPEG_ROOT}/lib
    )

    find_library(SWSCALE_LIBRARY
        NAMES swscale
        PATHS
            /opt/homebrew/lib
            /usr/local/lib
            ${FFMPEG_ROOT}/lib
    )

    if(FFMPEG_INCLUDE_DIR AND AVCODEC_LIBRARY AND AVFORMAT_LIBRARY AND AVUTIL_LIBRARY AND SWSCALE_LIBRARY)
        set(FFMPEG_FOUND TRUE)
        set(FFMPEG_LIBRARIES ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY})
        set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})

        # Create imported targets
        add_library(FFMPEG::avcodec UNKNOWN IMPORTED)
        set_target_properties(FFMPEG::avcodec PROPERTIES
            IMPORTED_LOCATION ${AVCODEC_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${FFMPEG_INCLUDE_DIR}
        )

        add_library(FFMPEG::avformat UNKNOWN IMPORTED)
        set_target_properties(FFMPEG::avformat PROPERTIES
            IMPORTED_LOCATION ${AVFORMAT_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${FFMPEG_INCLUDE_DIR}
        )

        add_library(FFMPEG::avutil UNKNOWN IMPORTED)
        set_target_properties(FFMPEG::avutil PROPERTIES
            IMPORTED_LOCATION ${AVUTIL_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${FFMPEG_INCLUDE_DIR}
        )

        add_library(FFMPEG::swscale UNKNOWN IMPORTED)
        set_target_properties(FFMPEG::swscale PROPERTIES
            IMPORTED_LOCATION ${SWSCALE_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${FFMPEG_INCLUDE_DIR}
        )

        message(STATUS "Found FFmpeg libraries:")
        message(STATUS "  Include: ${FFMPEG_INCLUDE_DIR}")
        message(STATUS "  avcodec: ${AVCODEC_LIBRARY}")
        message(STATUS "  avformat: ${AVFORMAT_LIBRARY}")
        message(STATUS "  avutil: ${AVUTIL_LIBRARY}")
        message(STATUS "  swscale: ${SWSCALE_LIBRARY}")
    else()
        set(FFMPEG_FOUND FALSE)
        message(FATAL_ERROR "FFmpeg libraries not found. Please install via: brew install ffmpeg")
    endif()
else()
    # For non-Apple platforms, try pkg-config or find_package
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale)
    else()
        message(FATAL_ERROR "FFmpeg not found and pkg-config not available")
    endif()
endif()
