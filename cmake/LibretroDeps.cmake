include(FetchContent)

# zstd - build from bundled single-file decoder shipped by libchdr.
# libchdr now ships a stripped-down decoder (zstd-1.5.7/CMakeLists.txt) that
# builds a target simply named "zstd"; the libchdr wrapper expects the
# vcpkg-style "zstd::libzstd_static" alias, so create it here.
FetchContent_Declare(zstd
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/libchdr/libchdr/deps/zstd-1.5.7
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(zstd)
if (TARGET zstd AND NOT TARGET zstd::libzstd_static)
    add_library(zstd::libzstd_static ALIAS zstd)
endif ()

# miniz - build from bundled source shipped by libchdr.
# The libchdr wrapper now requires miniz (find_package(miniz CONFIG REQUIRED))
# and links against miniz::miniz.
FetchContent_Declare(miniz
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/libchdr/libchdr/deps/miniz-3.1.1
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(miniz)
if (TARGET miniz AND NOT TARGET miniz::miniz)
    add_library(miniz::miniz ALIAS miniz)
endif ()

# fmt - fetch from GitHub
set(FMT_DOC OFF)
set(FMT_TEST OFF)
FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
    OVERRIDE_FIND_PACKAGE
)
