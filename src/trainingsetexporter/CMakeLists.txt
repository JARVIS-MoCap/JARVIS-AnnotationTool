project (trainingsetexporter)


add_library(${PROJECT_NAME}
  trainingsetexporter.hpp
  trainingsetexporter.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../../
    ../
    ../../libs/json_cpp
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-sign-compare -Wunused)
