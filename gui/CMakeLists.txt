project (gui)

add_subdirectory(Qt-DarkStyle)
add_subdirectory(videocutter)
add_subdirectory(newdatasetwindow)
add_subdirectory(newcalibrationwidget)
add_subdirectory(exportdatasetwidget)
add_subdirectory(editor)


add_library(${PROJECT_NAME}
    mainwindow.hpp
    mainbar.hpp
    settingswindow.hpp
    loaddatasetwindow.hpp
    mainwindow.cpp
    mainbar.cpp
    settingswindow.cpp
    loaddatasetwindow.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
	Qt5::Charts
  editor
	src
  newcalibrationwidget
  exportdatasetwidget
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
