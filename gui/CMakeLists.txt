add_subdirectory(Qt-DarkStyle)
add_subdirectory(newdatasetwindow)
add_subdirectory(newcalibrationwidget)
add_subdirectory(exporttrainingsetwidget)
add_subdirectory(editor)
add_subdirectory(conveniencewidgets)

add_library(gui
    mainwindow.hpp
    mainwindow.cpp
    mainbar.hpp
    mainbar.cpp
    settingswindow.hpp
    settingswindow.cpp
)

target_include_directories(gui
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    editor
    exporttrainingsetwidget
    newcalibrationwidget
    newdatasetwindow
)

target_link_libraries(gui
  Qt::Widgets
  src
  editor
  newdatasetwindow
  newcalibrationwidget
  exporttrainingset
  conveniencewidgets
)
