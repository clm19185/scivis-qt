#pragma once

#include <QWidget>
#include "ScatterData.h"

// Qt Widgets frontend for ScatterData.
// Handles rendering via QPainter and mouse interaction.
class ScatterWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ScatterWidget(ScatterData* data, QWidget* parent = nullptr);
  
protected:
  void paintEvent(QPaintEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
   
private:
  ScatterData* m_scatterData; // backend

  // Mouse cursor state
  QPointF m_cursor;        // cursor position in screen coordinates
  QPointF m_cursorDataPos; // cursor position in data coordinates
  int m_cursorClass = -1;  // predicted class (-1 = cursor outside widget)
  
  // Display constants
  static constexpr int   k_windowSize = 600;
  static constexpr float k_pointRadius = 4.0f;
  static constexpr int k_margin = 40;
  // Axes drawing constants
  static constexpr float k_tickInterval  = 1.0f;  // graduation every unit
  static constexpr float k_labelInterval = 2.0f;  // label every 2 units
  static constexpr float k_tickSize      = 4.0f;  // tick length in pixels
  // Class color table indexed by class label
  static const QColor k_classColors[];

  QColor classColor(int cls) const;
  
  // Coordinate transforms between data space and screen space
  QPointF dataToScreen(float x, float y) const;
  QPointF screenToData(const QPointF& screen) const;

  // Rendering helpers called from paintEvent
  void drawAxes(QPainter& painter) const;
  void drawGrid(QPainter& painter) const;
  void drawPoints(QPainter& painter) const;
  void drawCursor(QPainter& painter) const;
  void drawLegend(QPainter& painter) const;
  void drawTitle(QPainter& painter) const;

#ifdef ENABLE_TESTS
  friend class TstScatterWidget;
#endif
};
