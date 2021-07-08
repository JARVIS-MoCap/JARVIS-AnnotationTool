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
    newdatasetwindow.hpp
    configurableitemlist.hpp
    reprojectionwidget.hpp
		reprojectionchartwidget.hpp
		reprojectionchartview.hpp
		datasetcontrolwidget.hpp
    presetswindow.hpp
    mainwindow.cpp
    mainbar.cpp
    settingswindow.cpp
		editorwidget.cpp
		keypointwidget.cpp
		imageviewer.cpp
    loaddatasetwindow.cpp
    newdatasetwindow.cpp
    configurableitemlist.cpp
    reprojectionwidget.cpp
		reprojectionchartwidget.cpp
		reprojectionchartview.cpp
		datasetcontrolwidget.cpp
    presetswindow.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
	Qt5::Charts

	src
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
