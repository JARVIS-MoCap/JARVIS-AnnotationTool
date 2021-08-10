project (datasetcreator)


add_library(${PROJECT_NAME}
  datasetcreator.hpp
  imagewriter.hpp
  videostreamer.hpp
  datasetcreator.cpp
  imagewriter.cpp
  videostreamer.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../../
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
