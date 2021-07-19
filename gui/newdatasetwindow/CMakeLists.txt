project (newdatasetwindow)

add_library(${PROJECT_NAME}
  newdatasetwindow.hpp
  recordingstable.hpp
  configurableitemlist.hpp
  presetswindow.hpp
  newdatasetwindow.cpp
  configurableitemlist.cpp
  recordingstable.cpp
  presetswindow.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
	src
  videocutter
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
