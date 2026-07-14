find_package(Qt6 REQUIRED COMPONENTS Test)

#test ScatterData
qt_add_executable(tst_scatterdata
  ../src/ScatterData.cpp
  tst_scatterdata.cpp
)
target_include_directories(tst_scatterdata PRIVATE ../src)
target_compile_definitions(tst_scatterdata PRIVATE
  QT_NO_KEYWORDS
  ENABLE_TESTS
)
target_link_libraries(tst_scatterdata PRIVATE
  Qt6::Test
  Qt6::Core
  ${TORCH_LIBRARIES}
)
#target_precompile_headers(tst_scatterdata REUSE_FROM scivis-qt)

#test ScatterWidget
if(SCIVISQT_ENABLE_WIDGETS)
  qt_add_executable(tst_scatterwidget
    ../src/ScatterData.cpp
    ../src/ScatterWidget.cpp
    tst_scatterwidget.cpp
)
target_include_directories(tst_scatterwidget PRIVATE ../src)
target_compile_definitions(tst_scatterwidget PRIVATE
  QT_NO_KEYWORDS
  ENABLE_TESTS
  ENABLE_WIDGETS
)
target_link_libraries(tst_scatterwidget PRIVATE
  Qt6::Test
  Qt6::Core
  Qt6::Widgets
  ${TORCH_LIBRARIES}
)
#target_precompile_headers(tst_scatterwidget REUSE_FROM scivis-qt)
endif()

add_custom_target(run_tests
  COMMAND $<TARGET_FILE:tst_scatterwidget>
  COMMAND $<TARGET_FILE:tst_scatterdata>
  DEPENDS tst_scatterwidget tst_scatterdata
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
 











