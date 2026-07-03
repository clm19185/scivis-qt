#include "MainWindow.h"
#include <QToolBar>
#include <QFileDialog>
#include <QCoreApplication>

MainWindow::MainWindow(ScatterData* data, QWidget* parent)
  : QMainWindow(parent),
    m_data(data),
    m_scatterWidget(new ScatterWidget(data, this))
    
{
  setCentralWidget(m_scatterWidget);
  setupToolbar();
}

void MainWindow::setupToolbar()
{
    QToolBar* toolbar = addToolBar("Main");
    
    QAction* loadModelAction = toolbar->addAction("Load Model");
    QAction* loadDataAction  = toolbar->addAction("Load Data");
    
    connect(loadModelAction, &QAction::triggered, this, &MainWindow::onLoadModel);
    connect(loadDataAction,  &QAction::triggered, this, &MainWindow::onLoadData);
}

void MainWindow::onLoadModel()
{
  QString path = QFileDialog::getOpenFileName(this, "Load Model", QCoreApplication::applicationDirPath() + "/../models", "TorchScript Model (*.pt)");
  if (path.isEmpty()) return; // dialog cancelled
  m_data->loadModel(path);
}

void MainWindow::onLoadData()
{
  QString path = QFileDialog::getOpenFileName(this, "Load Data", QCoreApplication::applicationDirPath() + "/../data", "CSV Files (*.csv)");
  if (path.isEmpty()) return; // dialog cancelled
  m_data->loadData(path);
}
