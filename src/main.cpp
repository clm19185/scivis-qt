#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "ScatterWidget.h"
#include "ScatterData.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Backend. Single instance shared between Qt Widgets and QML frontends
  ScatterData data;

  // QML engine. context must be set before loading QML,
    // so signals emitted during data loading are received by QML
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("scatterData", &data);
  engine.load(QUrl("qrc:/ScivisQt/qml/Main.qml"));

  // Qt Widgets frontend
  ScatterWidget widget(&data);
  widget.setWindowTitle("ScivisQt - Widgets");

  // Load data after both frontends are ready
  data.loadModel("../models/model.pt");
  data.loadData("../data/points.csv");

  widget.show();

  // Quit application when last window is closed
  QObject::connect(&app, &QApplication::lastWindowClosed, &app, &QApplication::quit);
  
  return app.exec();
}
