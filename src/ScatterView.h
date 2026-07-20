#pragma once

#include <QQuickPaintedItem>
#include <qqmlregistration.h>
#include "ScatterData.h"

class ScatterView : public QQuickPaintedItem
{
  Q_OBJECT
  QML_ELEMENT
    
public:
  Q_PROPERTY(ScatterData* scatterData READ scatterData 
	     WRITE setScatterData NOTIFY scatterDataChanged)
  Q_PROPERTY(QVariantList classColors READ classColors 
	     WRITE setClassColors NOTIFY classColorsChanged)
  Q_PROPERTY(int margin READ margin       
	     WRITE setMargin NOTIFY marginChanged)
  Q_PROPERTY(float pointRadius  READ pointRadius  
	     WRITE setPointRadius  NOTIFY pointRadiusChanged)
  Q_PROPERTY(float gridOpacity  READ gridOpacity  
	     WRITE setGridOpacity  NOTIFY gridOpacityChanged)
  
  explicit ScatterView(QQuickItem* parent = nullptr);

  ScatterData* scatterData() const { return m_data; }
  QVariantList classColors() const { return m_classColors; }
  int margin() const { return m_margin; }
  float pointRadius() const { return m_pointRadius; }
  float gridOpacity() const { return m_gridOpacity; }
  
  void setScatterData(ScatterData* data);
  void setClassColors(const QVariantList& colors);
  void setMargin(int margin);
  void setPointRadius(float radius);
  void setGridOpacity(float opacity);
  
  void paint(QPainter* painter) override;

protected:
  void hoverMoveEvent(QHoverEvent* event) override;
  void hoverLeaveEvent(QHoverEvent* event) override;
    
Q_SIGNALS:
  void scatterDataChanged();
  void classColorsChanged();
  void marginChanged();
  void pointRadiusChanged();
  void gridOpacityChanged();

private:
  ScatterData* m_data = nullptr;  // backend

  // Mouse cursor state
  QPointF m_cursor;        // cursor position in screen coordinates
  QPointF m_cursorDataPos; // cursor position in data coordinates
  int m_cursorClass = -1;  // predicted class (-1 = cursor outside widget)
  
  // Display constants
  static constexpr int   k_windowSize = 600;
  float m_pointRadius = 4.0f;
  int m_margin = 40;
  float m_gridOpacity = 0.31f;
  // Axes drawing constants
  static constexpr float k_tickInterval  = 1.0f;  // graduation every unit
  static constexpr float k_labelInterval = 2.0f;  // label every 2 units
  static constexpr float k_tickSize      = 4.0f;  // tick length in pixels
  // Class color table indexed by class label
  QVariantList m_classColors;

  QColor classColor(int cls) const;

  // Coordinate transforms between data space and screen space
  QPointF dataToScreen(float x, float y) const;
  QPointF screenToData(const QPointF& screen) const;

  // Rendering helper
  void drawAxes(QPainter* painter) const;
  void drawGrid(QPainter* painter) const;
  void drawPoints(QPainter* painter) const;
  void drawCursor(QPainter* painter) const;
  void drawLegend(QPainter* painter) const;
  void drawTitle(QPainter* painter) const;
};
