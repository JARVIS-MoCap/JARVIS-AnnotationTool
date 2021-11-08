project (newdatasetwindow)


add_subdirectory(videocutter)

add_library(${PROJECT_NAME}
  newdatasetwindow.hpp
  newdatasetwindow.cpp
  recordingstable.hpp
  recordingstable.cpp
  datasetprogressinfowindow.hpp
  datasetprogressinfowindow.cpp
  skeletontablewidget.hpp
  skeletontablewidget.cpp
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
  videocutter
  datasetcreator
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused -Wno-sign-compare)
