add_library(videocutter
    videocutterwindow.hpp
    rangeslider.hpp
    timeline.hpp
    videocutterwindow.cpp
    rangeslider.cpp
    timeline.cpp
)

target_include_directories(videocutter
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(videocutter
  Qt::Widgets
  Qt::Multimedia
  Qt::MultimediaWidgets
	src
)
