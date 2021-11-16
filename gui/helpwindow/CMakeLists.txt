add_library(helpwindow
  helpwindow.hpp
  helpwindow.cpp
)

target_include_directories(helpwindow
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
    ../../
)

target_link_libraries(helpwindow
  Qt5::Widgets
)
