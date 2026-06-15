#include "ScatterWidget.h"
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMouseEvent>
#include <QFont>
#include <QFontMetrics>

ScatterWidget::ScatterWidget(QWidget* parent)
    : QWidget(parent)
{
  setFixedSize(k_windowSize, k_windowSize);
  setMouseTracking(true); // receive mouseMoveEvent without button press
  setCursor(Qt::CrossCursor);
}

void ScatterWidget::loadData(const QString& csvPath)
{
  QFile file(csvPath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qWarning() << "Cannot open file: " << csvPath;
    return;
  }
  
  QTextStream in(&file);
  in.readLine(); // skip CSV header

  while (!in.atEnd())
  {
    QString line = in.readLine();
    QStringList parts = line.split(',');
    if (parts.size() != 3) continue;

    Point p;
    p.x     = parts[0].toFloat();
    p.y     = parts[1].toFloat();
    p.label = parts[2].toInt();
    m_points.push_back(p);
  }

  qDebug() << "Loaded " << m_points.size() << "points";
}

void ScatterWidget::loadModel(const QString& modelPath)
{
  try{
    m_model = torch::jit::load(modelPath.toStdString());
        m_modelLoaded = true;
        qDebug() << "Model loaded: " << modelPath;
  } catch (const c10::Error& e) {
    qWarning() << "Failed to load model: " << e.what();
  }
  computeGrid();
}

void ScatterWidget::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.fillRect(rect(), Qt::black);

  // Decision region grid — precomputed, one color per cell
  if (!m_grid.empty()) {
    const float cellW = (float)(width()  - 2 * k_margin) / k_gridSize;
    const float cellH = (float)(height() - 2 * k_margin) / k_gridSize;
    for (int row = 0; row < k_gridSize; ++row) {
      for (int col = 0; col < k_gridSize; ++col) {
	int cls = m_grid[row * k_gridSize + col];
	QColor color = (cls == 0) ? QColor(0, 0, 150, 80) : QColor(150, 0, 0, 80);
	painter.fillRect(QRectF(k_margin + col * cellW, k_margin + row * cellH, cellW, cellH), color);
      }
    }
  }
  
  // Data points — colored by ground truth label
  if (m_points.empty()) return;

  for (const auto& p : m_points) {
    QPointF pos = dataToScreen(p.x, p.y);
    painter.setBrush(p.label == 0 ? Qt::blue : Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(pos, k_pointRadius, k_pointRadius);
  }

  // Cursor tooltip — coordinates and predicted class color
  if (m_cursorClass >= 0) {
    QColor color = m_cursorClass == 0 ? Qt::blue : Qt::red;
    
    QString line1 = QString("(%1, %2)")
      .arg(m_cursorDataPos.x(), 0, 'f', 2)
      .arg(m_cursorDataPos.y(), 0, 'f', 2);
    
    painter.setFont(QFont("Monospace", 9));
    QFontMetrics fm(painter.font());
    int textW = fm.horizontalAdvance(line1) + 8;
    int textH = fm.height() + 4;
    int boxX = m_cursor.x() < width()  / 2 ? m_cursor.x() + 10 : m_cursor.x() - textW - 10;
    int boxY = m_cursor.y() < height() / 2 ? m_cursor.y() + 10 : m_cursor.y() - textH - 10;

    QRect box(boxX, boxY, textW, textH);
    painter.setBrush(Qt::NoBrush);
    painter.fillRect(box, color);
    painter.setPen(QPen(Qt::white, 1));
    painter.drawRect(box);
    painter.drawText(box.adjusted(4, 2, -4, -2), Qt::AlignLeft | Qt::AlignTop, line1);
  }
  
  // Title
  painter.setPen(Qt::white);
  painter.setFont(QFont("Monospace", 11, QFont::Bold));
  painter.drawText(QRect(0, 0, width(), k_margin),
		   Qt::AlignHCenter | Qt::AlignVCenter,
		   "Decision boundary — LibTorch + Qt");

  // Legend
  painter.setFont(QFont("Monospace", 9));
  int legendX = k_margin;
  int legendY = height() - k_margin / 2;

  painter.setBrush(Qt::blue);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(QPointF(legendX, legendY), 5, 5);
  painter.setPen(Qt::white);
  painter.drawText(QPointF(legendX + 10, legendY + 4), "Class 0");

  painter.setBrush(Qt::red);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(QPointF(legendX + 80, legendY), 5, 5);
  painter.setPen(Qt::white);
  painter.drawText(QPointF(legendX + 90, legendY + 4), "Class 1");
}

void ScatterWidget::mouseMoveEvent(QMouseEvent* event)
{
  m_cursor = event->position();
  m_cursorDataPos = screenToData(m_cursor.x(), m_cursor.y());
  m_cursorClass = predict(m_cursorDataPos.x(), m_cursorDataPos.y());
  update();
}

QPointF ScatterWidget::dataToScreen(float x, float y) const
{
  // Map data coordinates [k_xMin, k_xMax] x [k_yMin, k_yMax] to screen pixels
  // y axis is flipped: data y increases upward, screen y increases downward
  float screenX = k_margin + (x - k_xMin) / (k_xMax - k_xMin) * (width() - 2 * k_margin);
  float screenY = k_margin + (1.0f - (y - k_yMin) / (k_yMax - k_yMin)) * (height() - 2 * k_margin);

  return QPointF(screenX, screenY);
}

QPointF ScatterWidget::screenToData(float px, float py) const
{
  // Inverse of dataToScreen
  float dataX = k_xMin + (px - k_margin) / (width()  - 2 * k_margin) * (k_xMax - k_xMin);
  float dataY = k_yMin + (1.0f - (py - k_margin) / (height() - 2 * k_margin)) * (k_yMax - k_yMin);
  return QPointF(dataX, dataY);
}

int ScatterWidget::predict(float x, float y)
{
  if (!m_modelLoaded) return -1;
  torch::Tensor input = torch::tensor({{x, y}});
  auto output = m_model.forward({input}).toTensor();
  return output.argmax(1).item<int>();
}

void ScatterWidget::computeGrid()
{
  // Precompute inference for each cell center — stored row-major
  // Called once after model load; avoids per-frame inference overhead
  m_grid.resize(k_gridSize * k_gridSize);
  
  const float cellW = (float)(width()  - 2 * k_margin) / k_gridSize;
  const float cellH = (float)(height() - 2 * k_margin) / k_gridSize;
  
  for (int row = 0; row < k_gridSize; ++row) {
    for (int col = 0; col < k_gridSize; ++col) {
      QPointF dataPos = screenToData((col + 0.5f) * cellW, (row + 0.5f) * cellH);
      m_grid[row * k_gridSize + col] = predict(dataPos.x(), dataPos.y());
    }
  }
  qDebug() << "Inference grid computed (" << k_gridSize << "x" << k_gridSize << ")";
}
