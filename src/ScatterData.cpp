#include "ScatterData.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

ScatterData::ScatterData(QObject* parent)
    : QObject(parent)
{
}

void ScatterData::loadData(const QString& csvPath)
{
  QFile file(csvPath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qWarning() << "Cannot open file: " << csvPath;
    return;
  }
  
  QTextStream in(&file);
  in.readLine(); // skip CSV header

  m_points.clear();
  while (!in.atEnd())
  {
    QString line = in.readLine();
    QStringList parts = line.split(',');
    if (parts.size() != 3) continue;

    // Each point stored as a QVariantMap for direct use by both C++ and QML
    QVariantMap point;
    point["x"]     = parts[0].toFloat();
    point["y"]     = parts[1].toFloat();
    point["label"] = parts[2].toInt();
    m_points.append(point);
  }

  qDebug() << "Loaded " << m_points.size() << "points";

  Q_EMIT pointsChanged();
}

void ScatterData::loadModel(const QString& modelPath)
{
  try{
    m_model = torch::jit::load(modelPath.toStdString());
    m_modelLoaded = true;
    qDebug() << "Model loaded: " << modelPath;
  } catch (const c10::Error& e) {
    qWarning() << "Failed to load model: " << e.what();
    return;
  }
  computeGrid();
}

int ScatterData::predict(float x, float y)
{
  if (!m_modelLoaded) return -1;

  // Single-point inference — input shape (1, 2), output shape (1, 2)
  torch::Tensor input = torch::tensor({{x, y}});
  auto output = m_model.forward({input}).toTensor();

  // Return index of highest-scoring class
  return output.argmax(1).item<int>();
}

void ScatterData::computeGrid()
{
  // Precompute inference for each cell center — stored row-major
  // Called once after model load; avoids per-frame inference overhead
  m_grid.clear();

  // Cell size in data coordinates
  const float cellW = (float) (k_xMax - k_xMin) / k_gridSize;
  const float cellH = (float) (k_yMax - k_yMin) / k_gridSize;

  // Iterate row-major: row 0 is top (y = k_yMax), row increases downward
  for (int row = 0; row < k_gridSize; ++row) {
    for (int col = 0; col < k_gridSize; ++col) {
      // Center of cell in data coordinates
      float dataX = k_xMin + (col + 0.5f) * cellW;
      float dataY = k_yMax - (row + 0.5f) * cellH;
    
      m_grid.append(predict(dataX, dataY));
    }
  }
  qDebug() << "Inference grid computed (" << k_gridSize << "x" << k_gridSize << ")";
  Q_EMIT gridChanged();
}
