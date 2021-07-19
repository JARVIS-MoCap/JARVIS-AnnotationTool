project (editor)

add_library(${PROJECT_NAME}
  editorwidget.hpp
  keypointwidget.hpp
  imageviewer.hpp
  reprojectionwidget.hpp
  reprojectionchartwidget.hpp
  reprojectionchartview.hpp
  datasetcontrolwidget.hpp
  editorwidget.cpp
  keypointwidget.cpp
  imageviewer.cpp
  reprojectionwidget.cpp
  reprojectionchartwidget.cpp
  reprojectionchartview.cpp
  datasetcontrolwidget.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  Qt5::Charts
  newdatasetwindow
	src
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
