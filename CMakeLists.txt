cmake_minimum_required(VERSION 3.16)

project(ScivisQt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "~/Software/libtorch")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Qml Quick)
find_package(Torch REQUIRED)


qt_add_executable(scivis-qt
  src/main.cpp
  src/ScatterWidget.cpp
  src/ScatterData.cpp
  )

qt_add_qml_module(scivis-qt
  URI ScivisQt
  VERSION 1.0
  QML_FILES
      qml/Main.qml
  )

target_compile_definitions(scivis-qt PRIVATE QT_NO_KEYWORDS)
target_link_libraries(scivis-qt PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::Qml
  Qt6::Quick
  ${TORCH_LIBRARIES}
)
