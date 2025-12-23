#include "MainWindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QKeyEvent>
#include <QLabel>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_currentPage(0) {
  setWindowTitle("CtrlP");
  resize(800, 600);
  setupUI();
  updateStatusBar();
}

void MainWindow::setupUI() {
  // Create central widget
  QWidget *central = new QWidget(this);
  central->setStyleSheet("background-color: black;");
  setCentralWidget(central);

  // Adding place holder for PDF
  QVBoxLayout *layout = new QVBoxLayout(central);
  QLabel *placeHolder = new QLabel("Press 'q' to quit", central);
  placeHolder->setStyleSheet("color: white; font-size: 24px;");
  placeHolder->setAlignment(Qt::AlignCenter);
  layout->addWidget(placeHolder);

  // Status Bar
  statusBar()->showMessage("Ready");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Q:
    QApplication::quit();
    break;

  case Qt::Key_J:
    nextPage();
    break;

  case Qt::Key_K:
    previousPage();
    break;

  default:
    QMainWindow::keyPressEvent(event);
  }
}

bool MainWindow::loadDocument(const QString &filePath) {
  if (m_document.load(filePath)) {
    m_currentPage = 0;
    updateStatusBar();

    // Update window title with document name
    QString fileName = QFileInfo(filePath).fileName();
    setWindowTitle(QString("CtrlP - %1").arg(fileName));

    return true;
  } else {
    statusBar()->showMessage("Error: " + m_document.errorString());
    return false;
  }
}

void MainWindow::updateStatusBar() {
  if (!m_document.isLoaded()) {
    statusBar()->showMessage("No document loaded");
    return;
  }

  int totalPages = m_document.pageCount();
  int displayPage = m_currentPage + 1;

  QSizeF sizeMM = m_document.pageSizeMM(m_currentPage);
  QString paperSize = Document::detectPaperSize(sizeMM);

  QString msg = QString(" [%1/%2] | %3 x %4 mm (%5)")
                    .arg(displayPage)
                    .arg(totalPages)
                    .arg(sizeMM.width(), 0, 'f', 1) // 1 decimal place
                    .arg(sizeMM.height(), 0, 'f', 1)
                    .arg(paperSize);

  statusBar()->showMessage(msg);
}

void MainWindow::nextPage() {
  if (!m_document.isLoaded())
    return;

  if (m_currentPage < m_document.pageCount() - 1) {
    m_currentPage++;
    updateStatusBar();
  }
}

void MainWindow::previousPage() {
  if (!m_document.isLoaded())
    return;

  if (m_currentPage > 0) {
    m_currentPage--;
    updateStatusBar();
  }
}
