find_package(Qt6 REQUIRED COMPONENTS Test Widgets)

# tst_scatterwidget
qt_add_executable(tst_scatterwidget
  tst_scatterwidget.cpp
  ../src/ScatterData.cpp
  ../src/ScatterWidget.cpp
)

target_include_directories(tst_scatterwidget PRIVATE ../src)

target_compile_definitions(tst_scatterwidget PRIVATE 
  QT_NO_KEYWORDS
  ENABLE_WIDGETS
  ENABLE_TESTS
)

target_link_libraries(tst_scatterwidget PRIVATE
  Qt6::Test
  Qt6::Widgets
  ${TORCH_LIBRARIES}
)

target_precompile_headers(tst_scatterwidget REUSE_FROM scivis-qt)


