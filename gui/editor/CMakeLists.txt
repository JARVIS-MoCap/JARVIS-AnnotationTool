project (editor)


add_library(${PROJECT_NAME}
  editorwidget.hpp
  editorwidget.cpp
  keypointwidget.hpp
  keypointwidget.cpp
  imageviewer.hpp
  imageviewer.cpp
  reprojectionwidget.hpp
  reprojectionwidget.cpp
  reprojectionchartwidget.hpp
  reprojectionchartwidget.cpp
  reprojectionchartview.hpp
  reprojectionchartview.cpp
  datasetcontrolwidget.hpp
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
