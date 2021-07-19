project (videocutter)

add_library(${PROJECT_NAME}
    videocutterwindow.hpp
    rangeslider.hpp
    timeline.hpp
    videocutterwindow.cpp
    rangeslider.cpp
    timeline.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  Qt5::Multimedia
  Qt5::MultimediaWidgets
	src
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
