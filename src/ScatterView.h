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
  
  explicit ScatterView(QQuickItem* parent = nullptr);
  ScatterData* scatterData() const { return m_data; }
  void setScatterData(ScatterData* data);
  void paint(QPainter* painter) override;
    
Q_SIGNALS:
  void scatterDataChanged();

private:
    ScatterData* m_data = nullptr;
};
