project (exporttrainingset)

add_library(${PROJECT_NAME}
  exporttrainingsetwidget.hpp
  exporttrainingsetwidget.cpp
  compositionchartview.hpp
  compositionchartview.cpp
)
message ("Source ${PROJECT_SOURCE_DIR}")


target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  Qt5::Charts
	src
  conveniencewidgets
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wunused)
