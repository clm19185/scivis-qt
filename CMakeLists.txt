cmake_minimum_required(VERSION 3.16)

project(ScivisQt)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "~/Software/libtorch")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Torch REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(scivisQt
  src/main.cpp
  src/ScatterWidget.cpp
  )
target_link_libraries(scivisQt
  Qt6::Core
  Qt6::Widgets
  ${TORCH_LIBRARIES}
)
target_compile_definitions(scivisQt PRIVATE QT_NO_KEYWORDS)
