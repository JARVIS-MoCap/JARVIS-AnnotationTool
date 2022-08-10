cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(AnnotationTool VERSION 1.1.0  LANGUAGES CXX)

# Version String
# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the latest commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get working tree status (dirty)
execute_process(
  COMMAND git diff --quiet
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  RESULT_VARIABLE GIT_DIRTY_RET
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(GIT_DIRTY_RET)
  set(VERSION_STRING "(git)${GIT_BRANCH}:${GIT_COMMIT_HASH}-dirty")
else()
  set(VERSION_STRING "git ${GIT_BRANCH}:${GIT_COMMIT_HASH}")
endif()
add_definitions("-DVERSION_STRING=\"${VERSION_STRING}\"")
message(STATUS "VERSION_STRING " ${VERSION_STRING})


set (CMAKE_PREFIX_PATH "libs/Qt5/qt_install/lib/cmake/")
find_package(Qt6  QUIET PATHS "libs/Qt5/qt_install/lib/cmake/" REQUIRED COMPONENTS Core Widgets Charts Multimedia MultimediaWidgets 3DCore 3DRender 3DExtras)

add_definitions("-DQT_DEBUG")


if(NOT WIN32)
	set(OpenCV_DIR "libs/OpenCV/opencv_install/lib/cmake/opencv4")
	set(OpenCV_STATIC ON)
else()
	set(OpenCV_DIR "libs/OpenCV/opencv_install")
endif()

find_package(OpenCV REQUIRED)
message(STATUS "OPENCV_DIR ${OpenCV_DIR}")


set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")

add_subdirectory(libs/yaml-cpp/)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (NOT (WIN32 OR APPLE))
  find_package(Eigen3 REQUIRED)
endif()


add_subdirectory(libs)
add_subdirectory(gui)
add_subdirectory(src)

if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
  set(ICON_NAME "icon.icns")
  set(ICON_PATH "${PROJECT_SOURCE_DIR}/IconThemes/DarkIconTheme/${ICON_NAME}")
  set(hand_ICON ${ICON_PATH})
  set_source_files_properties(${hand_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  file(COPY ${ICON_PATH} DESTINATION "${PROJECT_NAME}.app/Contents/Resources")
  file(COPY "${PROJECT_SOURCE_DIR}/IconThemes" DESTINATION "${PROJECT_NAME}.app/Contents/Resources")

  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${ICON_PATH} "${PROJECT_NAME}.app/Contents/Resources"
      main.cpp
      globals.hpp
      gui/Qt-DarkStyle/darkstyle.qrc
  )
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICONFILE ${ICON_NAME})

elseif(WIN32)
  set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/IconThemes/DarkIconTheme/appicon.rc")
  add_executable(${PROJECT_NAME}
      main.cpp
      globals.hpp
      gui/Qt-DarkStyle/darkstyle.qrc
      ${APP_ICON_RESOURCE_WINDOWS}
  )
  
elseif (UNIX)
  set(CMAKE_INSTALL_RPATH /usr/local/lib/JARVIS-AnnotationTool;)
  add_executable(${PROJECT_NAME}
      main.cpp
      globals.hpp
      gui/Qt-DarkStyle/darkstyle.qrc
  )
	if (${UBUNTU_VERSION})
	  if(${UBUNTU_VERSION} STREQUAL "2004")
	    install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/deployment/deb_packages/JARVIS-AnnotationTool_1.1-1_amd64_2004/usr/local/bin)
	  else()
	    install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/deployment/deb_packages/JARVIS-AnnotationTool_1.1-1_amd64_1804/usr/local/bin)
	  endif()
	endif()
endif()

target_compile_options(${PROJECT_NAME} PRIVATE)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
)


target_link_libraries(${PROJECT_NAME}
	Qt::Widgets
	Qt::Charts
	Qt::Multimedia
	Qt::MultimediaWidgets

	gui
	src
	darkstyle
	yaml-cpp
)
