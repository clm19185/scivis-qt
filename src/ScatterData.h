#pragma once

#include <QObject>
#include <QVariant>
#include <vector>
#include <torch/script.h>

// Backend data layer — shared between frontends (Qt Widgets and QML).
// Owns the dataset, the TorchScript model and the precomputed inference grid.
class ScatterData : public QObject
{
  Q_OBJECT

 public:
  // Q_PROPERTY declarations — accessible from QML
  Q_PROPERTY(QVariantList points READ points NOTIFY pointsChanged)
  Q_PROPERTY(int numClasses READ numClasses NOTIFY modelChanged)
  Q_PROPERTY(QVariantList grid READ grid NOTIFY modelChanged)
  Q_PROPERTY(int gridSize READ gridSize CONSTANT)
  Q_PROPERTY(float xMin READ xMin CONSTANT)
  Q_PROPERTY(float xMax READ xMax CONSTANT)
  Q_PROPERTY(float yMin READ yMin CONSTANT)
  Q_PROPERTY(float yMax READ yMax CONSTANT)
  
  explicit ScatterData(QObject* parent = nullptr);

  // Load dataset from CSV file (x, y, label)
  Q_INVOKABLE void loadData(const QString& csvPath);

  // Load TorchScript model and precompute inference grid
  Q_INVOKABLE void loadModel(const QString& modelPath);

  // Property getters — used by both QML and C++ frontends
  QVariantList points() const { return m_points; }
  int numClasses() const  { return m_numClasses; }
  QVariantList grid() const { return m_grid; }
  int gridSize() const { return k_gridSize; }
  float xMin() const { return k_xMin; }
  float xMax() const { return k_xMax; }
  float yMin() const { return k_yMin; }
  float yMax() const { return k_yMax; }

  // Run model inference on a single point in data coordinates
  // Q_INVOKABLE makes this callable directly from QML
  Q_INVOKABLE int predict(float x, float y); 

Q_SIGNALS:
    void pointsChanged(); // emitted after loadData
    void modelChanged();   // emitted after loadModel
    
 private:
  QVariantList m_points; // dataset: list of {x, y, label} maps
  QVariantList m_grid;   // inference grid: flattened row-major, size k_gridSize * k_gridSize
  torch::jit::script::Module m_model;
  bool m_modelLoaded = false;
  int m_numClasses = 0;
  
  // Data space bounds — define the visible region for all frontends
  static constexpr float k_xMin = -6.0f;
  static constexpr float k_xMax =  6.0f;
  static constexpr float k_yMin = -6.0f;
  static constexpr float k_yMax =  6.0f;
  static constexpr int   k_gridSize = 100;
  
  // Precompute inference for each cell of the grid
  // Called once after model is loaded — avoids per-frame inference overhead
  void computeGrid();
};
