# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include(GNUInstallDirs)

add_library(beman.monadics)
add_library(beman::monadics ALIAS beman.monadics)

target_sources(beman.monadics PRIVATE monadics.cpp)

target_sources(
    beman.monadics
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/beman/monadics/monadics.hpp
)

set_target_properties(
    beman.monadics
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON EXPORT_NAME monadics
)

install(
    TARGETS beman.monadics COMPONENT beman.monadics
    EXPORT beman.monadics
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}$<$<CONFIG:Debug>:/debug>
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}$<$<CONFIG:Debug>:/debug>
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(BEMAN_monadics_INSTALL_CONFIG_FILE_PACKAGE)
    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beman.monadics-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/beman.monadics"
        PATH_VARS PROJECT_NAME PROJECT_VERSION
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/beman.monadics"
        COMPONENT beman.monadics
    )

    install(
        EXPORT beman.monadics
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/beman.monadics"
        NAMESPACE beman::
        FILE beman.monadics-targets.cmake
        COMPONENT beman.monadics
    )
endif()
