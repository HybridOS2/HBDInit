set(HBDInit_OUTPUT_NAME hbdinit)

list(APPEND HBDInit_PRIVATE_INCLUDE_DIRECTORIES
)

list(APPEND HBDInit_UNIFIED_SOURCE_LIST_FILES
)

list(APPEND HBDInit_SOURCES
)

list(APPEND HBDInit_LIBRARIES
    -lpthread
)

if (ENABLE_SOCKET_STREAM)
    list(APPEND HBDInit_SYSTEM_INCLUDE_DIRECTORIES
    )

    list(APPEND HBDInit_LIBRARIES
    )
endif ()


