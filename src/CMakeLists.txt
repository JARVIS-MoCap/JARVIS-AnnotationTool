project (src)


add_subdirectory(calibrationtool)
add_subdirectory(datasetcreator)
add_subdirectory(trainingsetexporter)

add_library(${PROJECT_NAME}
	keypoint.hpp
	keypoint.cpp
	colormap.hpp
	colormap.cpp
	dataset.hpp
	dataset.cpp
	reprojectiontool.hpp
	reprojectiontool.cpp
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

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-sign-compare -Wunused)
