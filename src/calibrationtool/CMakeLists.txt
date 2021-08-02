project (calibrationtool)


add_library(${PROJECT_NAME}
  calibrationtool.hpp
  intrinsicscalibrator.hpp
  extrinsicscalibrator.hpp
  calibrationtool.cpp
  intrinsicscalibrator.cpp
  extrinsicscalibrator.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../../libs/libcbdetect/src
    ../../
    ../
)


target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  cbdetect
  tbb
  opencv_core
  opencv_calib3d
  opencv_videoio
  opencv_imgproc
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
