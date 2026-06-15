#pragma once

#include <QWidget>
#include <vector>
#include <torch/script.h>

// Custom QWidget displaying a 2D scatter plot with a neural network decision boundary.
// Loads a TorchScript model and a CSV dataset, renders the decision regions via a
// precomputed inference grid, and provides real-time inference at the mouse cursor.
class ScatterWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ScatterWidget(QWidget* parent = nullptr);
  void loadData(const QString& csvPath);
  void loadModel(const QString& modelPath);

protected:
  void paintEvent(QPaintEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
    
private:
  // 2D data point with class label
  struct Point
  {
    float x, y;
    int label;
  };
  
  std::vector<Point> m_points;
  torch::jit::script::Module m_model;
  bool m_modelLoaded = false;

  // Precomputed inference grid — flattened row-major, size k_gridSize * k_gridSize
  std::vector<int> m_grid;

  // Mouse cursor state
  QPointF m_cursor;        // cursor position in screen coordinates
  QPointF m_cursorDataPos; // cursor position in data coordinates
  int m_cursorClass = -1;  // predicted class (-1 = cursor outside widget)

  // Display constants
  static constexpr float k_xMin = -6.0f;
  static constexpr float k_xMax =  6.0f;
  static constexpr float k_yMin = -6.0f;
  static constexpr float k_yMax =  6.0f;
  static constexpr int   k_windowSize = 600;
  static constexpr float k_pointRadius = 4.0f;
  static constexpr int   k_gridSize = 100;
  static constexpr int k_margin = 40;

  // Coordinate transforms between data space and screen space
  QPointF dataToScreen(float x, float y) const;
  QPointF screenToData(float px, float py) const;

  // Run model inference on a single data point
  int predict(float x, float y);

  // Precompute inference grid — called once after model is loaded
  void computeGrid();
};
