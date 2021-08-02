project (src)


add_subdirectory(calibrationtool)

find_package(OpenCV REQUIRED)
message("OPENCV_DIR ${OpenCV_DIR}")

add_library(${PROJECT_NAME}
	keypoint.hpp
	colormap.hpp
	dataset.hpp
	datasetcreator.hpp
	reprojectiontool.hpp
	switch.hpp
	keypoint.cpp
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
	opencv_core
	opencv_calib3d
	opencv_videoio
	opencv_imgproc
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
