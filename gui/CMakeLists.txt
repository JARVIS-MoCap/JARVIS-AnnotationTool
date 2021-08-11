project (gui)


add_subdirectory(Qt-DarkStyle)
add_subdirectory(newdatasetwindow)
add_subdirectory(newcalibrationwidget)
add_subdirectory(exportdatasetwidget)
add_subdirectory(editor)
add_subdirectory(conveniencewidgets)
add_subdirectory(helpwindow)

add_library(${PROJECT_NAME}
    mainwindow.hpp
    mainwindow.cpp
    mainbar.hpp
    mainbar.cpp
    settingswindow.hpp
    settingswindow.cpp
    loaddatasetwindow.hpp
    loaddatasetwindow.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  src
  editor
  newdatasetwindow
  newcalibrationwidget
  exportdatasetwidget
  conveniencewidgets
  helpwindow
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused -Wno-sign-compare)
