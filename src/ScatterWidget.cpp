#include "ScatterWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFont>
#include <QFontMetrics>

ScatterWidget::ScatterWidget(ScatterData* data, QWidget* parent)
  :  QWidget(parent),
     m_scatterData(data)
{
  setFixedSize(k_windowSize, k_windowSize);
  setMouseTracking(true);  // receive mouseMoveEvent without button press
  setCursor(Qt::CrossCursor);
}

void ScatterWidget::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.fillRect(rect(), Qt::black);

  // Decision region grid. Precomputed  in ScatterData, one color per cell
  QVariantList grid = m_scatterData->grid();
  if (!grid.empty()) {
    int gridSize = m_scatterData->gridSize();
    const float cellW = (float)(width()  - 2 * k_margin) / gridSize;
    const float cellH = (float)(height() - 2 * k_margin) / gridSize;
    for (int row = 0; row < gridSize; ++row) {
      for (int col = 0; col < gridSize; ++col) {
	int cls = grid[row * gridSize + col].toInt();
	QColor color = (cls == 0) ? QColor(0, 0, 150, 80) : QColor(150, 0, 0, 80);
	painter.fillRect(QRectF(k_margin + col * cellW, k_margin + row * cellH, cellW, cellH), color);
      }
    }
  }
  
  // Data points. Colored by ground truth label
  QVariantList points = m_scatterData->points();
  if (points.empty()) return;

  for (const auto& pt : points) {
    QVariantMap p = pt.toMap();
    QPointF pos = dataToScreen(p["x"].toFloat(), p["y"].toFloat());
    painter.setBrush(p["label"].toInt() == 0 ? Qt::blue : Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(pos, k_pointRadius, k_pointRadius);
  }

  // Cursor tooltip. Data coordinates and predicted class color
  if (m_cursorClass >= 0) {
    QColor color = m_cursorClass == 0 ? Qt::blue : Qt::red;
    
    QString line1 = QString("(%1, %2)")
      .arg(m_cursorDataPos.x(), 0, 'f', 2)
      .arg(m_cursorDataPos.y(), 0, 'f', 2);
    
    painter.setFont(QFont("Monospace", 9));
    QFontMetrics fm(painter.font());
    int textW = fm.horizontalAdvance(line1) + 8;
    int textH = fm.height() + 4;

    // Position tooltip to stay within widget bounds
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
  m_cursorDataPos = screenToData(event->position());
  m_cursorClass = m_scatterData->predict(m_cursorDataPos.x(), m_cursorDataPos.y());
  update();
}

QPointF ScatterWidget::dataToScreen(float x, float y) const
{
  // Map data coordinates to screen pixels
  // y axis is flipped: data y increases upward, screen y increases downward
  float screenX = k_margin + (x - m_scatterData->xMin()) / (m_scatterData->xMax() - m_scatterData->xMin()) * (width()  - 2 * k_margin);
  float screenY = k_margin + (1.0f - (y - m_scatterData->yMin()) / (m_scatterData->yMax() - m_scatterData->yMin())) * (height() - 2 * k_margin);
  return QPointF(screenX, screenY);
}

QPointF ScatterWidget::screenToData(const QPointF& screen) const
{
  // Inverse of dataToScreen
  float dataX = m_scatterData->xMin() + (screen.x() - k_margin) / (width()  - 2 * k_margin) * (m_scatterData->xMax() - m_scatterData->xMin());
  float dataY = m_scatterData->yMin() + (1.0f - (screen.y() - k_margin) / (height() - 2 * k_margin)) * (m_scatterData->yMax() - m_scatterData->yMin());
  return QPointF(dataX, dataY);
}


