add_library(calibrationtool
  calibrationtool.hpp
  intrinsicscalibrator.hpp
  extrinsicscalibrator.hpp
  calibrationtool.cpp
  intrinsicscalibrator.cpp
  extrinsicscalibrator.cpp
)

target_include_directories(calibrationtool
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../../libs/libcbdetect/src
    ../../
    ../
)


target_link_libraries(calibrationtool
  Qt5::Widgets
  cbdetect
  opencv_core
  opencv_calib3d
  opencv_videoio
  opencv_imgproc
  opencv_aruco
  #opencv_highgui
)
