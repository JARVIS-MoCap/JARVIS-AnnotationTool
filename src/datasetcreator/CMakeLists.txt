add_library(datasetcreator
  datasetcreator.hpp
  datasetcreator.cpp
  imagewriter.hpp
  imagewriter.cpp
  videostreamer.hpp
  videostreamer.cpp
)

target_include_directories(datasetcreator
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
    ../../
    ../../libs/json_cpp

)

target_link_libraries(datasetcreator
  Qt5::Widgets
  opencv_core
  opencv_calib3d
  opencv_videoio
  opencv_imgproc
  yaml-cpp
)
