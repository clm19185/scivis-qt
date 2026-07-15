#include "ScatterView.h"
#include <QPainter>

ScatterView::ScatterView(QQuickItem* parent)
  : QQuickPaintedItem(parent)
{
}

void ScatterView::setScatterData(ScatterData* data)
{
  if (m_data == data) return;
  m_data = data;
  Q_EMIT scatterDataChanged();
  update();
}

void ScatterView::paint(QPainter* painter)
{
  painter->fillRect(boundingRect(), Qt::black);
}
