add_library(exporttrainingset
  exporttrainingsetwidget.hpp
  exporttrainingsetwidget.cpp
  compositionchartview.hpp
  compositionchartview.cpp
  annotationstatisticschartview.hpp
  annotationstatisticschartview.cpp
  datasetlist.hpp
  datasetlist.cpp
  segmentselectorwindow.hpp
  segmentselectorwindow.cpp
)

target_include_directories(exporttrainingset
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../conveniencewidgets
    ../
)

target_link_libraries(exporttrainingset
  Qt5::Widgets
  Qt5::Charts
	src
  conveniencewidgets
  trainingsetexporter
)
