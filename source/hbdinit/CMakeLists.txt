include(GlobalCommon)
include(MyMacros.cmake)

configure_file(include/hbdinit_version.h.in ${FORWARDING_HEADERS_DIR}/hbdinit_version.h @ONLY)
configure_file(include/hbdinit_features.h.in ${FORWARDING_HEADERS_DIR}/hbdinit_features.h @ONLY)

HBDINIT_EXECUTABLE_DECLARE(mginit)

list(APPEND mginit_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
    "${HBDINIT_DIR}"
    "${HBDINIT_DIR}/include"
    "${FORWARDING_HEADERS_DIR}"
    "${PURC_INCLUDE_DIRS}"
    "${HBDBUS_INCLUDE_DIRS}"
    "${GLIB_INCLUDE_DIRS}"
)

set(mginit_DEFINITIONS
)

HBDINIT_EXECUTABLE(mginit)

set(mginit_PLATFORM_INDEPENDENT_DIRS
    "utils"
    "mginit"
)

APPEND_ALL_SOURCE_FILES_IN_DIRLIST(mginit_SOURCES "${mginit_PLATFORM_INDEPENDENT_DIRS}")

list(APPEND mginit_SOURCES
    "${HBDINIT_DIR}/ports/vasprintf.c"
)

HBDINIT_INCLUDE_CONFIG_FILES_IF_EXISTS()

set(mginit_LIBRARIES
    MiniGUI::MiniGUI
    "${GLIB_LIBRARIES}"
    -lpthread
)

set_target_properties(mginit PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

HBDINIT_COMPUTE_SOURCES(mginit)
HBDINIT_FRAMEWORK(mginit)

install(TARGETS mginit DESTINATION "${EXEC_INSTALL_DIR}")

set(IMG_FILES
    "assets/img/logo.jpg"
    "assets/img/logo-cn.jpg"
)

install(FILES ${IMG_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/share/hbd-init/assets/img/")



