cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(AnnotationTool VERSION 0.2.0  LANGUAGES CXX)

#find_package(Qt5 COMPONENTS Widgets Core Gui Charts REQUIRED)

if(APPLE)
  set(OS_LIB_DIR "MacOS")
elseif(WIN32)
  set(OS_LIB_DIR "Windows")
elseif(UNIX)
  if(${CMAKE_SYSTEM} MATCHES "Linux-5.8")
    set(OS_LIB_DIR "Ubuntu_2004")
  else()
    set(OS_LIB_DIR "Ubuntu_1804")
  endif()
endif()

#set(CMAKE_PREFIX_PATH "libs/${STATIC_LIB_LOCATION}")

set(Qt5_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5")
set(Qt5Core_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Core")
set(Qt5Widgets_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Widgets")
set(Qt5Charts_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Charts")
set(Qt5Multimedia_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5Multimedia")
set(Qt5MultimediaWidgets_DIR "libs/Qt_5.15.2/${OS_LIB_DIR}/lib/cmake/Qt5MultimediaWidgets")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)
message (STATUS "QT DIRECTORY ${Qt5Core_DIR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT WIN32)
	set(OpenCV_DIR "libs/OpenCV_4.5.1/${OS_LIB_DIR}/lib/cmake/opencv4")
else()
	set(OpenCV_DIR "libs/OpenCV_4.5.1/${OS_LIB_DIR}")
endif()
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)
message(STATUS "OPENCV_DIR ${OpenCV_DIR}")

if(NOT WIN32)
	set(Eigen3_DIR "libs/eigen-3.3.8/MacOS/build")
	message("${EIGEN3_DIR}")
	find_package(Eigen3 3.3 REQUIRED NO_MODULE)
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
	Qt5::Charts
  Qt5::Multimedia
  Qt5::MultimediaWidgets
  gui
  src
  darkstyle
)
