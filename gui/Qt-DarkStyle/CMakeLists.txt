project (darkstyle)
#set(CMAKE_AUTORCC ON)


add_library(${PROJECT_NAME}
  DarkStyle.cpp
  DarkStyle.hpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${PROJECT_SOURCE_DIR}
  darkstyle
)

target_link_libraries(${PROJECT_NAME}
  Qt::Widgets
)
