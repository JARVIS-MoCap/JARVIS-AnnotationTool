project (newcalibrationwidget)

add_library(${PROJECT_NAME}
  newcalibrationwidget.hpp
  extrinsicspairlist.hpp
  calibrationprogressinfowindow.hpp
  newcalibrationwidget.cpp
  extrinsicspairlist.cpp
  calibrationprogressinfowindow.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
	src
  gui
  calibrationtool
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
