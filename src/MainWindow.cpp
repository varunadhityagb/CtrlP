#include "MainWindow.h"
#include <QApplication>
#include <QBoxLayout>
#include <QFileInfo>
#include <QFrame>
#include <QKeyEvent>
#include <QLabel>
#include <QLayoutItem>
#include <QList>
#include <QPixmap>
#include <QScrollArea>
#include <QScrollBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <climits>
#include <qnamespace.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_currentPage(0), m_dpi(150.0), m_pageGap(20),
      m_scrollAmount(100), m_showPageBoundaries(true),
      m_pageBoundaryColor(68, 68, 68), m_scrollArea(nullptr),
      m_contentWidget(nullptr), m_contentLayout(nullptr) {
  setWindowTitle("CtrlP");
  resize(800, 600);
  setupUI();
  updateStatusBar();
}

void MainWindow::setupUI() {
  m_contentWidget = new QWidget();
  m_contentWidget->setStyleSheet("background-color: black;");

  m_contentLayout = new QVBoxLayout(m_contentWidget);
  m_contentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  m_contentLayout->setSpacing(0);
  m_contentLayout->setContentsMargins(0, 20, 0, 20);

  m_scrollArea = new QScrollArea(this);
  m_scrollArea->setWidget(m_contentWidget);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setStyleSheet("background-color: black; border: none;");
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  setCentralWidget(m_scrollArea);

  statusBar()->showMessage("No document loaded");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {

  bool shift = event->modifiers() & Qt::ShiftModifier;

  switch (event->key()) {
  case Qt::Key_Q:
    QApplication::quit();
    break;

  case Qt::Key_J:
    if (shift)
      jumpToPage(m_currentPage + 1);
    else
      scrollBy(m_scrollAmount);
    break;

  case Qt::Key_K:
    if (shift)
      jumpToPage(m_currentPage - 1);
    else
      scrollBy(-m_scrollAmount);

    break;

  case Qt::Key_Z:
  case Qt::Key_Zoom:
    if (shift)
      zoomOut();
    else
      zoomIn();

    break;

  case Qt::Key_W:
    fitToWidth();
    break;

  case Qt::Key_H:
    fitToHeight();
    break;

  default:
    QMainWindow::keyPressEvent(event);
  }
}

bool MainWindow::loadDocument(const QString &filePath) {
  if (m_document.load(filePath)) {
    m_currentPage = 0;
    updateStatusBar();
    renderAllPages();

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

void MainWindow::renderAllPages() {
  if (!m_document.isLoaded())
    return;

  for (QLabel *label : m_pageLabels) {
    label->clear();
  }
  m_pageLabels.clear();

  QLayoutItem *item;
  while ((item = m_contentLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }

  QApplication::processEvents();
  m_pageLabels.clear();

  int pageCount = m_document.pageCount();

  for (int i = 0; i < pageCount; i++) {
    QImage image = m_document.renderPage(i, m_dpi);

    if (image.isNull())
      continue;

    QLabel *pageLabel = new QLabel();
    pageLabel->setPixmap(QPixmap::fromImage(image));
    pageLabel->setAlignment(Qt::AlignCenter);

    m_contentLayout->addWidget(pageLabel);
    m_pageLabels.append(pageLabel);

    if (i < pageCount - 1) {
      m_contentLayout->addSpacing(m_pageGap);

      if (m_showPageBoundaries) {
        QFrame *separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setStyleSheet(
            QString("background-color: %1;").arg(m_pageBoundaryColor.name()));
        separator->setFixedHeight(1);
        separator->setMaximumWidth(image.width());
        m_contentLayout->addWidget(separator);
      }

      m_contentLayout->addSpacing(m_pageGap);
    }
  }

  m_currentPage = getCurrentVisiblePage();
  updateStatusBar();
}

void MainWindow::scrollBy(int pixels) {
  QScrollBar *vbar = m_scrollArea->verticalScrollBar();
  vbar->setValue(vbar->value() + pixels);

  m_currentPage = getCurrentVisiblePage();
  updateStatusBar();
}

void MainWindow::jumpToPage(int pageNumber) {
  if (pageNumber < 0 || pageNumber >= m_document.pageCount())
    return;

  if (pageNumber >= m_pageLabels.size())
    return;

  QLabel *targetLabel = m_pageLabels[pageNumber];
  m_scrollArea->ensureWidgetVisible(targetLabel, 0, 0);

  m_currentPage = pageNumber;
  updateStatusBar();
}

void MainWindow::zoomIn() {
  m_dpi *= 1.2;
  renderAllPages();
}

void MainWindow::zoomOut() {
  m_dpi *= 0.8;
  if (m_dpi < 50.0)
    m_dpi = 50.0;

  renderAllPages();
}

void MainWindow::fitToWidth() {
  if (!m_document.isLoaded())
    return;

  QSizeF pageSize = m_document.pageSize(m_currentPage);
  int windowWidth = m_scrollArea->viewport()->width() - 40;

  m_dpi = (windowWidth * 72) / pageSize.width();

  renderAllPages();
}

void MainWindow::fitToHeight() {
  if (!m_document.isLoaded())
    return;

  QSizeF pageSize = m_document.pageSize(m_currentPage);
  int windowHeight = m_scrollArea->viewport()->height() - 40;

  m_dpi = (windowHeight * 72.0) / pageSize.height();

  renderAllPages();
}

int MainWindow::getCurrentVisiblePage() {
  if (!m_document.isLoaded() || m_pageLabels.isEmpty())
    return 0;

  int scrollY = m_scrollArea->verticalScrollBar()->value();
  int viewportTop = scrollY;
  int viewportCenter = scrollY + m_scrollArea->viewport()->height() / 2;

  int closestPage = 0;
  int minDistance = INT_MAX;

  for (int i = 0; i < m_pageLabels.size(); i++) {
    QLabel *label = m_pageLabels[i];
    int labelTop = label->y();
    int labelCenter = labelTop + label->height() / 2;

    int distance = qAbs(labelCenter - viewportCenter);

    if (distance < minDistance) {
      minDistance = distance;
      closestPage = i;
    }
  }

  return closestPage;
}
