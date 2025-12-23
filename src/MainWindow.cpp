#include "MainWindow.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("CtrlP");
  resize(800, 600);
  createMenus();
}

void MainWindow::createMenus() {
  QMenu *fileMenu = menuBar()->addMenu("File");

  QAction *quitAction = new QAction("Quit", this);
  fileMenu->addAction(quitAction);

  QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
}
