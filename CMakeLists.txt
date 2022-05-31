cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)

project(AnnotationTool VERSION 1.1.0  LANGUAGES CXX)

if(APPLE)
  set(OS_LIB_DIR "MacOS")
elseif(WIN32)
  set(OS_LIB_DIR "Windows")
elseif(UNIX)
  message ("System ${CMAKE_SYSTEM}")
  if(${UBUNTU_VERSION} STREQUAL "2004")
    set(OS_LIB_DIR "Ubuntu_2004")
  else()
    set(OS_LIB_DIR "Ubuntu_1804")
  endif()
endif()

if (NOT UNIX)
  set(Qt5_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5")
  set(Qt5Core_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Core")
  set(Qt5Widgets_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Widgets")
  set(Qt5Charts_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Charts")
  set(Qt5Multimedia_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Multimedia")
  set(Qt5MultimediaWidgets_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5MultimediaWidgets")
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)
message (STATUS "QT DIRECTORY ${Qt5Core_DIR}")

if(NOT WIN32)
	set(OpenCV_DIR "libs/OpenCV_4.5.1/${OS_LIB_DIR}/lib/cmake/opencv4")
else()
	set(OpenCV_DIR "libs/OpenCV_4.5.1/${OS_LIB_DIR}")
endif()
find_package(OpenCV REQUIRED)
message(STATUS "OPENCV_DIR ${OpenCV_DIR}")


set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")

add_subdirectory(libs/yaml-cpp/)


find_package(OpenCV REQUIRED)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (NOT WIN32)
find_package(Eigen3 REQUIRED)
endif()


if(APPLE)
	#set(Eigen3_DIR "libs/eigen-3.3.8/MacOS/share/eigen3")
	#message("${EIGEN3_DIR}")
	#find_package(Eigen3 3.3 REQUIRED NO_MODULE)
endif()

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

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
  file(COPY "${PROJECT_SOURCE_DIR}/help" DESTINATION "${PROJECT_NAME}.app/Contents/Resources")

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
  set(CMAKE_INSTALL_RPATH /usr/local/lib/JARVIS-AnnotationTool;/usr/local/lib/JARVIS-AnnotationTool/OpenCV;)
  add_executable(${PROJECT_NAME}
      main.cpp
      globals.hpp
      gui/Qt-DarkStyle/darkstyle.qrc
  )

  if(${UBUNTU_VERSION} STREQUAL "2004")
    install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/deployment/deb_packages/JARVIS-AnnotationTool_0.3-1_amd64_2004/usr/local/bin)
  else()
    install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/deployment/deb_packages/JARVIS-AnnotationTool_0.3-1_amd64_1804/usr/local/bin)
  endif()
endif()

target_compile_options(${PROJECT_NAME} PRIVATE)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
)


target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
	Qt5::Charts
  Qt5::Multimedia
  Qt5::MultimediaWidgets
  gui
  src
  darkstyle
  yaml-cpp
)
