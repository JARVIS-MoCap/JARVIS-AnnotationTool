add_subdirectory(videocutter)

add_library(newdatasetwindow
  newdatasetwindow.hpp
  newdatasetwindow.cpp
  recordingstable.hpp
  recordingstable.cpp
  datasetprogressinfowindow.hpp
  datasetprogressinfowindow.cpp
  skeletontablewidget.hpp
  skeletontablewidget.cpp
)

target_include_directories(newdatasetwindow
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
    videocutter
    ${PROJECT_SOURCE_DIR}/src/datasetcreator
)

target_link_libraries(newdatasetwindow
  Qt::Widgets
	src
  gui
  videocutter
  datasetcreator
)
