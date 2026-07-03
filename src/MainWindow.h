#pragma once

#include <QMainWindow>
#include "ScatterData.h"
#include "ScatterWidget.h"

// Main application window — Qt Widgets frontend.
// Owns the toolbar and wraps ScatterWidget as the central widget.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(ScatterData* data, QWidget* parent = nullptr);

private:
  ScatterData*   m_data;
  ScatterWidget* m_scatterWidget;
    
  void setupToolbar();
  void loadPreset(const QString& name);

private Q_SLOTS:
  void onLoadModel();
  void onLoadData();
};
