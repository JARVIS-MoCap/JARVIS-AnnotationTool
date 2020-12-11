project (gui)

add_subdirectory(Qt-DarkStyle)

add_library(${PROJECT_NAME}
    mainwindow.hpp
    mainbar.hpp
    settingswindow.hpp
		editorwidget.hpp
		keypointwidget.hpp
		imageviewer.hpp
    loaddatasetwindow.hpp
    reprojectionwidget.hpp
		datasetcontrolwidget.hpp
    mainwindow.cpp
    mainbar.cpp
    settingswindow.cpp
		editorwidget.cpp
		keypointwidget.cpp
		imageviewer.cpp
    loaddatasetwindow.cpp
    reprojectionwidget.cpp
		datasetcontrolwidget.cpp
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

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
