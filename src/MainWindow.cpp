#include "MainWindow.h"
#include <QToolBar>
#include <QFileDialog>
#include <QCoreApplication>
#include <QStatusBar>

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

  toolbar->addSeparator();

  // Preset actions
  QAction* presetSeparated    = toolbar->addAction("Separated");
  QAction* presetOverlapping  = toolbar->addAction("Overlapping");
  QAction* presetMoons        = toolbar->addAction("Moons");
  QAction* presetThreeClasses = toolbar->addAction("3 Classes");

  connect(presetSeparated,    &QAction::triggered, this, [this]{ loadPreset("separated"); });
  connect(presetOverlapping,  &QAction::triggered, this, [this]{ loadPreset("overlapping"); });
  connect(presetMoons,        &QAction::triggered, this, [this]{ loadPreset("moons"); });
  connect(presetThreeClasses, &QAction::triggered, this, [this]{ loadPreset("three_classes"); });
}

void MainWindow::onLoadModel()
{
  QString path = QFileDialog::getOpenFileName(
     this,
     "Load Model",
     "../models",
     "TorchScript models (*.pt)"
  );
  if (path.isEmpty()) return; 
  m_data->loadModel(path);
  QFileInfo fileInfo(path);
  statusBar()->showMessage("Model: " + fileInfo.fileName());
}

void MainWindow::onLoadData()
{
  QString path = QFileDialog::getOpenFileName(
     this,
     "Load Data",
     "../data",
     "CSV Files (*.csv)"
  );
  if (path.isEmpty()) return; 
  m_data->loadData(path);
  QFileInfo fileInfo(path);
  statusBar()->showMessage("Data: " + fileInfo.fileName());
}

void MainWindow::loadPreset(const QString& name)
{
  m_data->loadModel("../models/" + name + ".pt");
  m_data->loadData("../data/" + name + ".csv");
  statusBar()->showMessage("Preset: " + name);
}


