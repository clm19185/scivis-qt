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

  // Display constants  — frontend-specific
  static constexpr int   k_windowSize = 600;
  static constexpr float k_pointRadius = 4.0f;
  static constexpr int k_margin = 40;

  // Coordinate transforms between data space and screen space
  QPointF dataToScreen(float x, float y) const;
  QPointF screenToData(const QPointF& screen) const;
};
