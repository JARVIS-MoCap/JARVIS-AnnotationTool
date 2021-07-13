project (src)


set(CMAKE_PREFIX_PATH "../libs/${STATIC_LIB_LOCATION}")

set(OpenCV_DIR "../libs/${STATIC_LIB_LOCATION}/opencv_static")
set(OpenCV_STATIC ON)

if(NOT WIN32)
	find_package(Eigen3 3.3 REQUIRED NO_MODULE)
endif()

find_package(OpenCV REQUIRED)
message("OPENCV_DIR ${OpenCV_DIR}")

add_library(${PROJECT_NAME}
	keypoint.hpp
	configreader.hpp
	colormap.hpp
	dataset.hpp
	datasetcreator.hpp
	reprojectiontool.hpp
	switch.hpp
	keypoint.cpp
	configreader.cpp
	colormap.cpp
	dataset.cpp
	datasetcreator.cpp
	reprojectiontool.cpp
	switch.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
	yaml-cpp
	opencv_core
	opencv_calib3d
	opencv_videoio
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
