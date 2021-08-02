project (newdatasetwindow)

add_subdirectory(videocutter)

add_library(${PROJECT_NAME}
  newdatasetwindow.hpp
  recordingstable.hpp
  presetswindow.hpp
  datasetprogressinfowindow.hpp
  newdatasetwindow.cpp
  recordingstable.cpp
  presetswindow.cpp
  datasetprogressinfowindow.cpp
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
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
