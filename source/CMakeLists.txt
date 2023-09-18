# -----------------------------------------------------------------------------
# Add components directories
# -----------------------------------------------------------------------------
add_subdirectory(wtf)

add_subdirectory(hbdinit)

if (ENABLE_API_TESTS)
    add_subdirectory(third-party/gtest)
#    add_subdirectory(test)
endif ()

#add_subdirectory(samples)

HBDINIT_INCLUDE_CONFIG_FILES_IF_EXISTS()
