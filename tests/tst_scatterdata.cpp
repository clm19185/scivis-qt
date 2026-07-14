#include <QtTest>
#include "ScatterData.h"

class TstScatterData : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  void init();
  void cleanup();

  void initTestCase() {};
  void cleanupTestCase() {};
  
  void tst_loadData_missing_file();
  void tst_loadData_count();
  void tst_loadModel_missing_file();
  void tst_predict_no_model();
  void tst_predict_valid();
  void tst_computeGrid_size();
  void tst_computeGrid_values();

private:
  ScatterData* m_data = nullptr;
};

void TstScatterData::init()
{
  m_data = new ScatterData();
}

void TstScatterData::cleanup()
{
  delete m_data;
}

void TstScatterData::tst_loadData_missing_file()
{
  // Loading a missing file should not crash and points stay empty
  m_data->loadData("../data/nonexistent.csv");
  QVERIFY(m_data->points().isEmpty());
}

void TstScatterData::tst_loadData_count()
{
  if (!QFile::exists("../data/separated.csv"))
    QSKIP("Data file not found — run generate_presets.py first");

  m_data->loadData("../data/separated.csv");
  QCOMPARE(m_data->points().size(), 200);
}

void TstScatterData::tst_loadModel_missing_file()
{
  // Loading a missing model should not crash
  m_data->loadModel("../models/nonexistent.pt");
}

void TstScatterData::tst_predict_no_model()
{
  // predict should return -1 when no model is loaded for any point
  QCOMPARE(m_data->predict(0.0f, 0.0f), -1);
}

void TstScatterData::tst_predict_valid()
{
  //load a model and check predict values for that model
  if (!QFile::exists("../models/separated.pt"))
    QSKIP("Model file not found — run generate_presets.py first");

  m_data->loadModel("../models/separated.pt");

  // Point clearly in class 0 zone should predict 0
  QCOMPARE(m_data->predict(-4.0f, -4.0f), 0);

  // Point clearly in class 1 zone should predict 1
  QCOMPARE(m_data->predict(4.0f, 4.0f), 1);
}

void TstScatterData::tst_computeGrid_size()
{
  if (!QFile::exists("../models/separated.pt"))
    QSKIP("Model file not found");

  m_data->loadModel("../models/separated.pt");

  int expected = m_data->gridSize() * m_data->gridSize();
  QCOMPARE(m_data->grid().size(), expected);
}

void TstScatterData::tst_computeGrid_values()
{
  if (!QFile::exists("../models/separated.pt"))
    QSKIP("Model file not found");

  m_data->loadModel("../models/separated.pt");

  // All grid values should be valid class indices
  for (const auto& v : m_data->grid()) {
    int cls = v.toInt();
    QVERIFY(cls >= 0 && cls < m_data->numClasses());
  }
}


QTEST_MAIN(TstScatterData)
#include "tst_scatterdata.moc"
