#ifdef ENABLE_QML
#include <QQmlContext>
#include <QQmlApplicationEngine>
#endif

#ifdef ENABLE_WIDGETS
#include "ScatterWidget.h"
#include <QApplication>
#else
#include <QGuiApplication>
#endif

#include "ScatterData.h"

int main(int argc, char *argv[])
{
#ifdef ENABLE_WIDGETS
    QApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif

  // Backend. Single instance shared between Qt Widgets and QML frontends
  ScatterData data;

#ifdef ENABLE_QML
  // QML engine. context must be set before loading QML,
    // so signals emitted during data loading are received by QML
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("scatterData", &data);
  engine.load(QUrl("qrc:/ScivisQt/qml/Main.qml"));
#endif

#ifdef ENABLE_WIDGETS
  // Qt Widgets frontend
  ScatterWidget widget(&data);
  widget.setWindowTitle("ScivisQt - Widgets");
#endif

  // Load data after both frontends are ready
  data.loadModel("../models/model.pt");
  data.loadData("../data/points.csv");


#ifdef ENABLE_WIDGETS
  widget.show();
#endif

  // Quit application when last window is closed
  QObject::connect(&app, &QGuiApplication::lastWindowClosed, &app, &QCoreApplication::quit);
  
  return app.exec();
}
