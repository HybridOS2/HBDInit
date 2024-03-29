set(WTF_PUBLIC_HEADERS
    Compiler.h
    ExportMacros.h
    Platform.h
    PlatformCallingConventions.h
    PlatformCPU.h
    PlatformEnable.h
    PlatformHave.h
    PlatformLegacy.h
    PlatformOS.h
    PlatformUse.h
)

set(WTF_SOURCES
    nothing.c
)

set(WTF_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
    "${WTF_DERIVED_SOURCES_DIR}"
    "${WTF_DIR}"
    "${WTF_DIR}/wtf"
)

set(WTF_LIBRARIES
    ${CMAKE_DL_LIBS}
)

#if (NOT USE_SYSTEM_MALLOC)
#    list(APPEND WTF_LIBRARIES HBDInit::bmalloc)
#endif ()

set(WTF_INTERFACE_LIBRARIES WTF)
set(WTF_INTERFACE_INCLUDE_DIRECTORIES ${WTF_FRAMEWORK_HEADERS_DIR})
set(WTF_INTERFACE_DEPENDENCIES WTF_CopyHeaders)

HBDINIT_FRAMEWORK_DECLARE(WTF)
HBDINIT_INCLUDE_CONFIG_FILES_IF_EXISTS()

HBDINIT_COPY_FILES(WTF_CopyHeaders
    DESTINATION ${WTF_FRAMEWORK_HEADERS_DIR}/wtf
    FILES ${WTF_PUBLIC_HEADERS}
)

HBDINIT_WRAP_SOURCELIST(${WTF_SOURCES})
HBDINIT_FRAMEWORK(WTF)
HBDINIT_FRAMEWORK_TARGET(WTF)

if (MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif ()

