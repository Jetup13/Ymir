add_subdirectory(ymir-core)

if (Ymir_ENABLE_YMIR_HEADLESS OR Ymir_ENABLE_YMIR_DBG OR Ymir_ENABLE_TESTS)
    add_subdirectory(ymir-dbg-commons)
endif ()
