cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)


project(AnnotationTool VERSION 1.0.0  LANGUAGES CXX) #removed CUDA here!!

#find_package(Qt5 COMPONENTS Widgets Core Gui Charts REQUIRED)

if(APPLE)
  set(STATIC_LIB_LOCATION "Apple")
elseif(WIN32)
  set(STATIC_LIB_LOCATION "Windows")
elseif(UNIX)
  set(STATIC_LIB_LOCATION "Linux")
endif()

set(CMAKE_PREFIX_PATH "libs/${STATIC_LIB_LOCATION}")

set(Qt5_DIR "libs/${STATIC_LIB_LOCATION}/Qt5static_v5.15.2/lib/cmake/Qt5")
set(Qt5Core_DIR "libs/${STATIC_LIB_LOCATION}/Qt5static_v5.15.2/lib/cmake/Qt5Core")
set(Qt5Widgets_DIR "libs/${STATIC_LIB_LOCATION}/Qt5static_v5.15.2/lib/cmake/Qt5Widgets")

#find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

find_package(Qt5Core)
find_package(Qt5Widgets)

message (STATUS "QT DIRECTORY ${Qt5Core_DIR}")


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

add_subdirectory(gui)
add_subdirectory(src)
add_subdirectory(libs)


if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
  set(ICON_NAME "icon.icns")
  set(ICON_PATH "${PROJECT_SOURCE_DIR}/icons/${ICON_NAME}")
  set(hand_ICON ${ICON_PATH})
  set_source_files_properties(${hand_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  file(COPY ${ICON_PATH} DESTINATION "${PROJECT_NAME}.app/Contents/Resources")
  file(COPY "${PROJECT_SOURCE_DIR}/icons" DESTINATION "${PROJECT_NAME}.app/Contents/Resources")

  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${ICON_PATH} "${PROJECT_NAME}.app/Contents/Resources"
      main.cpp
      globals.hpp
      gui/Qt-DarkStyle/darkstyle.qrc
  )

  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICONFILE ${ICON_NAME})
elseif(WIN32)
  add_executable(${PROJECT_NAME}
      main.cpp
      globals.hpp
      gui/Qt-DarkStyle/darkstyle.qrc
  )
elseif (UNIX)
  add_executable(${PROJECT_NAME}
      main.cpp
      globals.hpp
      gui/Qt-DarkStyle/darkstyle.qrc
  )
  install(TARGETS ${PROJECT_NAME})
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/icons DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  gui
  src
  darkstyle
)
