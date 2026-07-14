#include <QtTest>
#include "ScatterWidget.h"
#include "ScatterData.h"

class TstScatterWidget : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  void init();
  void cleanup();

  void initTestCase() {};
  void cleanupTestCase() {};
  
  void tst_dataToScreen_roundtrip();
  void tst_screenToData_roundtrip();

private:
  ScatterData*   m_data   = nullptr;
  ScatterWidget* m_widget = nullptr;
};

void TstScatterWidget::init()
{
  m_data   = new ScatterData();
  m_widget = new ScatterWidget(m_data);
}

void TstScatterWidget::cleanup()
{
  delete m_widget;
  delete m_data;
}

void TstScatterWidget::tst_dataToScreen_roundtrip()
{
  // dataToScreen followed by screenToData should return original point
  float x = 2.5f, y = -1.3f;
  QPointF screen = m_widget->dataToScreen(x, y);
  QPointF data   = m_widget->screenToData(screen);
  QVERIFY(qAbs(data.x() - x) < 0.01);
  QVERIFY(qAbs(data.y() - y) < 0.01);
}

void TstScatterWidget::tst_screenToData_roundtrip()
{
  //screenToData  followed by dataToScreen should return original point
  QPointF screen(250.0, 380.0);
  QPointF data   = m_widget->screenToData(screen);
  QPointF screen2 = m_widget->dataToScreen(data.x(), data.y());
  QVERIFY(qAbs(screen2.x() - screen.x()) < 0.01);
  QVERIFY(qAbs(screen2.y() - screen.y()) < 0.01);
}

QTEST_MAIN(TstScatterWidget)
#include "tst_scatterwidget.moc"
