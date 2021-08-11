project (datasetcreator)


add_library(${PROJECT_NAME}
  datasetcreator.hpp
  datasetcreator.cpp
  imagewriter.hpp
  imagewriter.cpp
  videostreamer.hpp
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

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-sign-compare -Wunused)
