add_library(visualizationwindow
  visualizationwindow.hpp
  visualizationwindow.cpp
  bettercameracontroller.hpp
  bettercameracontroller.cpp
  viewport.hpp
  viewport.cpp
  vistoolbar.hpp
  vistoolbar.cpp
  controldock.hpp
  controldock.cpp
)

target_include_directories(visualizationwindow
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/gui/conveniencewidgets
    ../
    ../../
)

target_link_libraries(visualizationwindow
  Qt::Widgets
  Qt::3DCore
  Qt::3DRender

  src
  conveniencewidgets
)
