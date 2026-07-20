#ifdef ENABLE_QML
#include <QQmlContext>
#include <QQmlApplicationEngine>
#endif

#ifdef ENABLE_WIDGETS
#include "MainWindow.h"
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
  engine.rootContext()->setContextProperty("scatterBackend", &data);
  engine.load(QUrl("qrc:/ScivisQt/qml/Main.qml"));
#endif

#ifdef ENABLE_WIDGETS
  MainWindow mainWindow(&data);
  mainWindow.setWindowTitle("ScivisQt - Widgets");
  mainWindow.show();
  mainWindow.move(100, 100);
  mainWindow.raise();
  //mainWindow.activateWindow();
#endif

  // Quit application when last window is closed
  QObject::connect(&app, &QGuiApplication::lastWindowClosed, &app, &QCoreApplication::quit);
  
  return app.exec();
}
