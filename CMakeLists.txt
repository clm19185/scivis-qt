cmake_minimum_required(VERSION 3.16)

project(ScivisQt)

option(SCIVISQT_ENABLE_WIDGETS "Enable Qt Widgets frontend" ON)
option(SCIVISQT_ENABLE_QML "Enable QML frontend" ON)

if(NOT SCIVISQT_ENABLE_WIDGETS AND NOT SCIVISQT_ENABLE_QML)
    message(FATAL_ERROR "At least one frontend (Widgets or QML) must be enabled")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "~/Software/libtorch")

set(QT_COMPONENTS Core Gui)
if(SCIVISQT_ENABLE_WIDGETS)
    list(APPEND QT_COMPONENTS Widgets)
endif()
if(SCIVISQT_ENABLE_QML)
    list(APPEND QT_COMPONENTS Qml Quick)
endif()

find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(Torch REQUIRED)

set(SCIVISQT_SOURCES
    src/main.cpp
    src/ScatterData.cpp
)
if(SCIVISQT_ENABLE_WIDGETS)
  list(APPEND SCIVISQT_SOURCES
    src/ScatterWidget.cpp)
endif()

qt_add_executable(scivis-qt ${SCIVISQT_SOURCES})

target_precompile_headers(scivis-qt PRIVATE
    <torch/script.h>
)

if(SCIVISQT_ENABLE_QML)
  qt_add_qml_module(scivis-qt
    URI ScivisQt
    VERSION 1.0
    QML_FILES
      qml/Main.qml
      )
endif()    

target_compile_definitions(scivis-qt PRIVATE QT_NO_KEYWORDS)

set(SCIVISQT_LINK_LIBS Qt6::Core Qt6::Gui ${TORCH_LIBRARIES})
if(SCIVISQT_ENABLE_WIDGETS)
    list(APPEND SCIVISQT_LINK_LIBS Qt6::Widgets)
endif()
if(SCIVISQT_ENABLE_QML)
    list(APPEND SCIVISQT_LINK_LIBS Qt6::Qml Qt6::Quick)
endif()

target_link_libraries(scivis-qt PRIVATE ${SCIVISQT_LINK_LIBS})

if(SCIVISQT_ENABLE_WIDGETS)
    target_compile_definitions(scivis-qt PRIVATE ENABLE_WIDGETS)
endif()
if(SCIVISQT_ENABLE_QML)
    target_compile_definitions(scivis-qt PRIVATE ENABLE_QML)
endif()
