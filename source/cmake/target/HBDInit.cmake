if (NOT TARGET HBDInit::HBDInit)
    if (NOT INTERNAL_BUILD)
        message(FATAL_ERROR "HBDInit::HBDInit target not found")
    endif ()

    # This should be moved to an if block if the Apple Mac/iOS build moves completely to CMake
    # Just assuming Windows for the moment
    add_library(HBDInit::HBDInit STATIC IMPORTED)
    set_target_properties(HBDInit::HBDInit PROPERTIES
        IMPORTED_LOCATION ${WEBKIT_LIBRARIES_LINK_DIR}/HBDInit${DEBUG_SUFFIX}.lib
    )
    set(HBDInit_PRIVATE_FRAMEWORK_HEADERS_DIR "${CMAKE_BINARY_DIR}/../include/private")
    target_include_directories(HBDInit::HBDInit INTERFACE
        ${HBDInit_PRIVATE_FRAMEWORK_HEADERS_DIR}
    )
endif ()
