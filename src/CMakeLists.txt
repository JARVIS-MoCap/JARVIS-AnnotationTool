add_subdirectory(calibrationtool)
add_subdirectory(datasetcreator)
add_subdirectory(trainingsetexporter)

add_library(src
	keypoint.hpp
	keypoint.cpp
	colormap.hpp
	colormap.cpp
	dataset.hpp
	dataset.cpp
	reprojectiontool.hpp
	reprojectiontool.cpp
)

target_include_directories(src
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)


target_link_libraries(src
  Qt::Widgets
	opencv_core
	opencv_calib3d
	opencv_videoio
	opencv_imgproc
	#opencv_highgui
)
