
add_library(Qt5::DmaBufServerBufferIntegrationPlugin MODULE IMPORTED)

set(_Qt5DmaBufServerBufferIntegrationPlugin_MODULE_DEPENDENCIES "WaylandCompositor;WaylandCompositor;Gui;Core")

foreach(_module_dep ${_Qt5DmaBufServerBufferIntegrationPlugin_MODULE_DEPENDENCIES})
    if(NOT Qt5${_module_dep}_FOUND)
        find_package(Qt5${_module_dep}
            1.0.0 ${_Qt5WaylandCompositor_FIND_VERSION_EXACT}
            ${_Qt5WaylandCompositor_DEPENDENCIES_FIND_QUIET}
            ${_Qt5WaylandCompositor_FIND_DEPENDENCIES_REQUIRED}
            PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
        )
    endif()
endforeach()

_qt5_WaylandCompositor_process_prl_file(
    "${_qt5WaylandCompositor_install_prefix}/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-dmabuf-server-buffer.prl" RELEASE
    _Qt5DmaBufServerBufferIntegrationPlugin_STATIC_RELEASE_LIB_DEPENDENCIES
    _Qt5DmaBufServerBufferIntegrationPlugin_STATIC_RELEASE_LINK_FLAGS
)


set_property(TARGET Qt5::DmaBufServerBufferIntegrationPlugin PROPERTY INTERFACE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Qt5WaylandCompositor_DmaBufServerBufferIntegrationPlugin_Import.cpp"
)

_populate_WaylandCompositor_plugin_properties(DmaBufServerBufferIntegrationPlugin RELEASE "wayland-graphics-integration-server/libqt-wayland-compositor-dmabuf-server-buffer.a" FALSE)

list(APPEND Qt5WaylandCompositor_PLUGINS Qt5::DmaBufServerBufferIntegrationPlugin)
set_property(TARGET Qt5::WaylandCompositor APPEND PROPERTY QT_ALL_PLUGINS_wayland_graphics_integration_server Qt5::DmaBufServerBufferIntegrationPlugin)
# $<GENEX_EVAL:...> wasn't added until CMake 3.12, so put a version guard around it
if(CMAKE_VERSION VERSION_LESS "3.12")
    set(_manual_plugins_genex "$<TARGET_PROPERTY:QT_PLUGINS>")
    set(_plugin_type_genex "$<TARGET_PROPERTY:QT_PLUGINS_wayland_graphics_integration_server>")
    set(_no_plugins_genex "$<TARGET_PROPERTY:QT_NO_PLUGINS>")
else()
    set(_manual_plugins_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_PLUGINS>>")
    set(_plugin_type_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_PLUGINS_wayland_graphics_integration_server>>")
    set(_no_plugins_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_NO_PLUGINS>>")
endif()
set(_user_specified_genex
    "$<IN_LIST:Qt5::DmaBufServerBufferIntegrationPlugin,${_manual_plugins_genex};${_plugin_type_genex}>"
)
set(_user_specified_genex_versionless
    "$<IN_LIST:Qt::DmaBufServerBufferIntegrationPlugin,${_manual_plugins_genex};${_plugin_type_genex}>"
)
string(CONCAT _plugin_genex
    "$<$<OR:"
        # Add this plugin if it's in the list of manually specified plugins or in the list of
        # explicitly included plugin types.
        "${_user_specified_genex},"
        "${_user_specified_genex_versionless},"
        # Add this plugin if all of the following are true:
        # 1) the list of explicitly included plugin types is empty
        # 2) the QT_PLUGIN_EXTENDS property for the plugin is empty or equal to the current
        #    module name
        # 3) the user hasn't explicitly excluded the plugin.
        "$<AND:"
            "$<STREQUAL:${_plugin_type_genex},>,"
            "$<OR:"
                # FIXME: The value of CMAKE_MODULE_NAME seems to be wrong (e.g for Svg plugin
                # it should be Qt::Svg instead of Qt::Gui).
                "$<STREQUAL:$<TARGET_PROPERTY:Qt5::DmaBufServerBufferIntegrationPlugin,QT_PLUGIN_EXTENDS>,Qt::WaylandCompositor>,"
                "$<STREQUAL:$<TARGET_PROPERTY:Qt5::DmaBufServerBufferIntegrationPlugin,QT_PLUGIN_EXTENDS>,>"
            ">,"
            "$<NOT:$<IN_LIST:Qt5::DmaBufServerBufferIntegrationPlugin,${_no_plugins_genex}>>,"
            "$<NOT:$<IN_LIST:Qt::DmaBufServerBufferIntegrationPlugin,${_no_plugins_genex}>>"
        ">"
    ">:Qt5::DmaBufServerBufferIntegrationPlugin>"
)
set_property(TARGET Qt5::WaylandCompositor APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    ${_plugin_genex}
)
set_property(TARGET Qt5::DmaBufServerBufferIntegrationPlugin APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    "Qt5::WaylandCompositor;Qt5::WaylandCompositor;Qt5::Gui;Qt5::Core"
)
set_property(TARGET Qt5::DmaBufServerBufferIntegrationPlugin PROPERTY QT_PLUGIN_TYPE "wayland-graphics-integration-server")
set_property(TARGET Qt5::DmaBufServerBufferIntegrationPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::DmaBufServerBufferIntegrationPlugin PROPERTY QT_PLUGIN_CLASS_NAME "DmaBufServerBufferIntegrationPlugin")
