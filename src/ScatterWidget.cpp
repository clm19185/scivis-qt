#include "ScatterWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFont>
#include <QFontMetrics>
#include <cmath>

// Okabe-Ito colorblind-friendly palette — Wong, B. Nature Methods 8:441 (2011)
// Last entry is the default color for unknown classes
const QColor ScatterWidget::k_classColors[] = {
    QColor("#0072B2"),  // class 0 — blue
    QColor("#D55E00"),  // class 1 — vermillion
    QColor("#009E73"),  // class 2 — bluish green
    QColor("#CC79A7")   // default — reddish purple
};

ScatterWidget::ScatterWidget(ScatterData* data, QWidget* parent)
  :  QWidget(parent),
     m_scatterData(data)
{
  setFixedSize(k_windowSize, k_windowSize);
  setMouseTracking(true);  // receive mouseMoveEvent without button press
  connect(m_scatterData, &ScatterData::pointsChanged, this, QOverload<>::of(&QWidget::update));
  connect(m_scatterData, &ScatterData::modelChanged,   this, QOverload<>::of(&QWidget::update));
}

void ScatterWidget::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.fillRect(rect(), Qt::black); //black background
  drawAxes(painter);
  drawGrid(painter);
  drawPoints(painter);
  drawCursor(painter);
  drawTitle(painter);
  drawLegend(painter);
}

void ScatterWidget::mouseMoveEvent(QMouseEvent* event)
{
  m_cursor = event->position();
  
  // Only show cursor tooltip inside the data area
  if (m_cursor.x() < k_margin || m_cursor.x() > width()  - k_margin ||
      m_cursor.y() < k_margin || m_cursor.y() > height() - k_margin)
  {
    m_cursorClass = -1;
    setCursor(Qt::ArrowCursor);
    update();
    return;
  }

  setCursor(Qt::CrossCursor);
  m_cursorDataPos = screenToData(event->position());
  m_cursorClass = m_scatterData->predict(m_cursorDataPos.x(), m_cursorDataPos.y());
  update();
}

QColor ScatterWidget::classColor(int cls) const
{
  if (cls < 0 || cls >= (int) std::size(k_classColors) - 1)
    return k_classColors[std::size(k_classColors) - 1];
  return k_classColors[cls];
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

void ScatterWidget::drawAxes(QPainter& painter) const
{
  painter.setPen(QPen(Qt::white, 1));
  painter.setFont(QFont("Monospace", 8));
  QFontMetrics fm(painter.font());

  // Axis lines through (0, 0) in data coordinates
  QPointF origin = dataToScreen(0.0f, 0.0f);
  QPointF xLeft  = dataToScreen(m_scatterData->xMin(), 0.0f);
  QPointF xRight = dataToScreen(m_scatterData->xMax(), 0.0f);
  QPointF yTop   = dataToScreen(0.0f, m_scatterData->yMax());
  QPointF yBot   = dataToScreen(0.0f, m_scatterData->yMin());

  painter.drawLine(xLeft, xRight);  // X axis
  painter.drawLine(yTop,  yBot);    // Y axis

  // Ticks, labels and grid lines
  for (float v = m_scatterData->xMin(); v <= m_scatterData->xMax(); v += k_tickInterval) {

    // Vertical grid line
    QPointF top = dataToScreen(v, m_scatterData->yMax());
    QPointF bot = dataToScreen(v, m_scatterData->yMin());
    painter.setPen(QPen(QColor(255, 255, 255, 30), 1));
    painter.drawLine(top, bot);

    // Tick on X axis
    QPointF tickPos = dataToScreen(v, 0.0f);
    painter.setPen(QPen(Qt::white, 1));
    painter.drawLine(QPointF(tickPos.x(), tickPos.y() - k_tickSize),
		     QPointF(tickPos.x(), tickPos.y() + k_tickSize));

    // Label every k_labelInterval units
    if (std::fmod(std::abs(v), k_labelInterval) < 0.01f && v != 0.0f) {
      QString label = QString::number(v, 'f', 1);
      int textW = fm.horizontalAdvance(label);
      painter.drawText(QPointF(tickPos.x() - textW / 2, tickPos.y() + k_tickSize + fm.height()), label);
    }
  }

  for (float v = m_scatterData->yMin(); v <= m_scatterData->yMax(); v += k_tickInterval) {

    // Horizontal grid line
    QPointF left  = dataToScreen(m_scatterData->xMin(), v);
    QPointF right = dataToScreen(m_scatterData->xMax(), v);
    painter.setPen(QPen(QColor(255, 255, 255, 30), 1));
    painter.drawLine(left, right);

    // Tick on Y axis
    QPointF tickPos = dataToScreen(0.0f, v);
    painter.setPen(QPen(Qt::white, 1));
    painter.drawLine(QPointF(tickPos.x() - k_tickSize, tickPos.y()),
		     QPointF(tickPos.x() + k_tickSize, tickPos.y()));

    // Label every k_labelInterval units
    if (std::fmod(std::abs(v), k_labelInterval) < 0.01f && v != 0.0f) {
      QString label = QString::number(v, 'f', 1);
      int textW = fm.horizontalAdvance(label);
      painter.drawText(QPointF(tickPos.x() - textW - k_tickSize - 2, tickPos.y() + fm.height() / 2), label);
    }
  }

  // Origin label
  painter.setPen(Qt::white);
  painter.drawText(QPointF(origin.x() + k_tickSize + 2, origin.y() + fm.height()), "0");
}

void ScatterWidget::drawGrid(QPainter& painter) const
{
  // Decision region grid is precomputed  in ScatterData with one color per cell
  QVariantList grid = m_scatterData->grid();
  if (!grid.empty()) {
    int gridSize = m_scatterData->gridSize();
    const float cellW = (float)(width()  - 2 * k_margin) / gridSize;
    const float cellH = (float)(height() - 2 * k_margin) / gridSize;
    for (int row = 0; row < gridSize; ++row) {
      for (int col = 0; col < gridSize; ++col) {
	int cls = grid[row * gridSize + col].toInt();
	QColor color = classColor(cls);
	color.setAlpha(80);
	painter.fillRect(QRectF(k_margin + col * cellW, k_margin + row * cellH, cellW, cellH), color);
      }
    }
  }
}

void ScatterWidget::drawPoints(QPainter& painter) const
{
  // Data points are colored by ground truth label
  QVariantList points = m_scatterData->points();
  if (points.empty()) return;

  for (const auto& pt : points) {
    QVariantMap p = pt.toMap();
    QPointF pos = dataToScreen(p["x"].toFloat(), p["y"].toFloat());
    painter.setBrush(classColor(p["label"].toInt()));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(pos, k_pointRadius, k_pointRadius);
  }
}

void ScatterWidget::drawCursor(QPainter& painter) const
{
  // Cursor tooltip has data coordinates and predicted class color
  if (m_cursorClass >= 0) {
    QColor color = classColor(m_cursorClass);
    
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
}

void ScatterWidget::drawTitle(QPainter& painter) const
{
  painter.setPen(Qt::white);
  painter.setFont(QFont("Monospace", 11, QFont::Bold));
  painter.drawText(QRect(0, 0, width(), k_margin),
		   Qt::AlignHCenter | Qt::AlignVCenter,
		   "Decision boundary — LibTorch + Qt");
}

void ScatterWidget::drawLegend(QPainter& painter) const
{
  painter.setFont(QFont("Monospace", 9));
  int legendX = k_margin;
  int legendY = height() - k_margin / 2;
  int numClasses = m_scatterData->numClasses();

  for (int cls = 0; cls < numClasses; ++cls) {
    int offsetX = cls * 80;
    painter.setBrush(classColor(cls));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(legendX + offsetX, legendY), 5, 5);
    painter.setPen(Qt::white);
    painter.drawText(QPointF(legendX + offsetX + 10, legendY + 4), QString("Class %1").arg(cls));
  }
}
