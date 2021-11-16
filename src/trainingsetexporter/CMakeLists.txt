project (trainingsetexporter)


add_library(trainingsetexporter
  trainingsetexporter.hpp
  trainingsetexporter.cpp
)

target_include_directories(trainingsetexporter
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../../
    ../
    ../../libs/json_cpp
)

target_link_libraries(trainingsetexporter
  Qt5::Widgets
)
