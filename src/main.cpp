#include <QApplication>
#include "ScatterWidget.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  ScatterWidget widget;
  widget.loadModel("../models/model.pt");
  widget.loadData("../data/points.csv");
  widget.setWindowTitle("ScivisQt");
  widget.show();

  // Quit application when main widget is closed
  QObject::connect(&widget, &QWidget::destroyed, &app, &QApplication::quit);
  
  return app.exec();
}
