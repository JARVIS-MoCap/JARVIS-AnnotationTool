project (conveniencewidgets)


add_library(${PROJECT_NAME}
  labelwithtooltip.hpp
  labelwithtooltip.cpp
  pathwidget.hpp
  pathwidget.cpp
  configurableitemlist.hpp
  configurableitemlist.cpp
  presetswindow.hpp
  presetswindow.cpp
  switch.hpp
  switch.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
	src
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused -Wno-sign-compare)
