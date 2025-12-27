#include "MainWindow.h"
#include "PrintSettings.h"
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
      m_pageBoundaryColor(68, 68, 68), m_printSettings(), m_scrollArea(nullptr),
      m_contentWidget(nullptr), m_contentLayout(nullptr), m_InputState(NORMAL),
      m_numberBuffer(""), m_commandInput(nullptr) {
  setWindowTitle("CtrlP");
  resize(800, 600);
  setupUI();

  m_keySequenceTimer = new QTimer(this);
  m_keySequenceTimer->setSingleShot(true);
  m_keySequenceTimer->setInterval(1000);
  connect(m_keySequenceTimer, &QTimer::timeout, this,
          &MainWindow::resetKeySequence);

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

  m_commandInput = new QLineEdit(this);
  m_commandInput->setStyleSheet("QLineEdit {"
                                "  background-color: #222222;"
                                "  color: white;"
                                "  border: none;"
                                "  padding: 2px 5px;"
                                "  font-family: monospace;"
                                "}");
  m_commandInput->setVisible(false);

  statusBar()->addPermanentWidget(m_commandInput);

  connect(m_commandInput, &QLineEdit::returnPressed, [this]() {
    executeCommand(m_commandInput->text());
    exitCommandMode();
  });

  statusBar()->showMessage("No document loaded");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (m_InputState == COMMAND_MODE) {
    if (event->key() == Qt::Key_Escape)
      exitCommandMode();
    return;
  }

  m_keySequenceTimer->stop();
  bool shift = event->modifiers() & Qt::ShiftModifier;
  int key = event->key();

  if (key >= Qt::Key_0 && key <= Qt::Key_9) {
    int digit = key - Qt::Key_0;
    handleNumberKey(digit);
    return;
  }

  switch (key) {
  case Qt::Key_Q:
    QApplication::quit();
    break;

  case Qt::Key_G:
    if (shift) {
      jumpToPage(m_document.pageCount() - 1);
    } else {
      if (m_InputState == AWAITING_G) {
        jumpToPage(0);
        resetKeySequence();
      } else {
        m_InputState = AWAITING_G;
        statusBar()->showMessage("g", 1000);
        m_keySequenceTimer->start();
      }
    }
    break;

  case Qt::Key_Colon:
    enterCommandMode();
    break;

  case Qt::Key_Escape:
    resetKeySequence();
    updateStatusBar();
    break;

  case Qt::Key_J:
    if (shift) {
      int count = 1;
      if (!m_numberBuffer.isEmpty()) {
        bool ok;
        count = m_numberBuffer.toInt(&ok);
        if (!ok || count <= 0)
          count = 1;
      }
      jumpToPage(m_currentPage + count);
    } else {
      // j: Scroll down (with optional multiplier)
      int count = 1;
      if (!m_numberBuffer.isEmpty()) {
        bool ok;
        count = m_numberBuffer.toInt(&ok);
        if (!ok || count <= 0)
          count = 1;
      }
      scrollBy(m_scrollAmount * count);
    }
    resetKeySequence(); // Clear buffer AFTER using it
    break;

  case Qt::Key_K:
    if (shift) {
      // Shift+K: Previous page (with optional multiplier)
      int count = 1;
      if (!m_numberBuffer.isEmpty()) {
        bool ok;
        count = m_numberBuffer.toInt(&ok);
        if (!ok || count <= 0)
          count = 1;
      }
      jumpToPage(m_currentPage - count);
    } else {
      // k: Scroll up (with optional multiplier)
      int count = 1;
      if (!m_numberBuffer.isEmpty()) {
        bool ok;
        count = m_numberBuffer.toInt(&ok);
        if (!ok || count <= 0)
          count = 1;
      }
      scrollBy(-m_scrollAmount * count);
    }
    resetKeySequence(); // Clear buffer AFTER using it
    break;

  case Qt::Key_Z:
    resetKeySequence();
    if (shift)
      zoomOut();
    else
      zoomIn();
    break;

  case Qt::Key_W:
    resetKeySequence();
    fitToWidth();
    break;

  case Qt::Key_H:
    resetKeySequence();
    fitToHeight();
    break;

  case Qt::Key_M:
    resetKeySequence();
    cycleMargniPreset();
    break;

  case Qt::Key_D:
    resetKeySequence();
    cycleDuplexMode();
    break;

  case Qt::Key_C:
    resetKeySequence();
    toggleColorMode();
    break;

  case Qt::Key_S:
    resetKeySequence();
    cycleScaleMode();
    break;

  default:
    if (event->modifiers() == Qt::NoModifier || key == Qt::Key_Shift ||
        key == Qt::Key_Control || key == Qt::Key_Alt || key == Qt::Key_Meta) {
      QMainWindow::keyPressEvent(event);
      break;
    }
    resetKeySequence();
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

  QLayoutItem *item;
  while ((item = m_contentLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }

  m_pageWidgets.clear();

  int pageCount = m_document.pageCount();

  for (int i = 0; i < pageCount; i++) {
    QImage image = m_document.renderPage(i, m_dpi);

    if (image.isNull())
      continue;

    if (!m_printSettings.colorMode) {
      image = image.convertToFormat(QImage::Format_Grayscale8);
    }

    PageWidget *pageWidget = new PageWidget();
    pageWidget->setPagePixmap(QPixmap::fromImage(image));
    pageWidget->setPrintSettings(&m_printSettings);
    pageWidget->setDPI(m_dpi);
    pageWidget->setPageNumber(i);

    m_contentLayout->addWidget(pageWidget);
    m_pageWidgets.append(pageWidget);

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

  if (pageNumber >= m_pageWidgets.size())
    return;

  PageWidget *targetWidget = m_pageWidgets[pageNumber];
  m_scrollArea->ensureWidgetVisible(targetWidget, 0, 0);

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
  if (!m_document.isLoaded() || m_pageWidgets.isEmpty())
    return 0;

  int scrollY = m_scrollArea->verticalScrollBar()->value();
  int viewportCenter = scrollY + m_scrollArea->viewport()->height() / 2;

  int closestPage = 0;
  int minDistance = INT_MAX;

  for (int i = 0; i < m_pageWidgets.size(); i++) {
    PageWidget *widget = m_pageWidgets[i];
    int widgetTop = widget->y();
    int widgetCenter = widgetTop + widget->height() / 2;

    int distance = qAbs(widgetCenter - viewportCenter);

    if (distance < minDistance) {
      minDistance = distance;
      closestPage = i;
    }
  }

  return closestPage;
}

void MainWindow::handleNumberKey(int digit) {
  m_numberBuffer += QString::number(digit);

  statusBar()->showMessage(":" + m_numberBuffer, 1000);

  m_keySequenceTimer->start();
}

void MainWindow::enterCommandMode() {
  m_InputState = COMMAND_MODE;
  m_commandInput->clear();
  m_commandInput->setText(":");
  m_commandInput->setVisible(true);
  m_commandInput->setFocus();
  m_commandInput->setCursorPosition(1);
}

void MainWindow::exitCommandMode() {
  m_InputState = NORMAL;
  m_commandInput->setVisible(false);
  m_commandInput->clear();
  setFocus();
  updateStatusBar();
}

void MainWindow::executeCommand(const QString &cmd) {
  QString command = cmd.trimmed();

  if (command.startsWith(":"))
    command = command.mid(1);

  if (command.isEmpty())
    return;

  bool ok;
  int pageNum = command.toInt(&ok);
  if (ok && pageNum > 0) {
    jumpToPage(pageNum - 1);
    return;
  }

  statusBar()->showMessage("Unknown command: " + command, 2000);
}

void MainWindow::resetKeySequence() {
  m_InputState = NORMAL;
  m_numberBuffer.clear();
  m_keySequenceTimer->stop();
}

void MainWindow::cycleMargniPreset() {
  QString current = m_printSettings.marginPresetName();

  if (current == "None")
    m_printSettings.margins = PrintSettings::marginPresetMinimal();
  else if (current == "Minimal")
    m_printSettings.margins = PrintSettings::marginPresetNormal();
  else if (current == "Normal")
    m_printSettings.margins = PrintSettings::marginPresetComfortable();
  else if (current == "Comfortable")
    m_printSettings.margins = PrintSettings::marginPresetWide();
  else
    m_printSettings.margins = PrintSettings::marginPresetNone();

  for (PageWidget *widget : m_pageWidgets)
    widget->update();

  statusBar()->showMessage(
      QString("Margins: %1").arg(m_printSettings.marginPresetName()), 2000);
}

void MainWindow::cycleDuplexMode() {
  switch (m_printSettings.duplexMode) {
  case PrintSettings::Simplex:
    m_printSettings.duplexMode = PrintSettings::DuplexLongEdge;
    break;
  case PrintSettings::DuplexLongEdge:
    m_printSettings.duplexMode = PrintSettings::DuplexShortEdge;
    break;
  case PrintSettings::DuplexShortEdge:
    m_printSettings.duplexMode = PrintSettings::Simplex;
    break;
  }

  for (PageWidget *widget : m_pageWidgets)
    widget->update();

  statusBar()->showMessage(
      QString("Duplex: %1").arg(m_printSettings.duplexModeName()), 2000);
}

void MainWindow::toggleColorMode() {
  m_printSettings.colorMode = !m_printSettings.colorMode;

  renderAllPages();

  statusBar()->showMessage(
      m_printSettings.colorMode ? "Color: On" : "Color: Off (Grayscale)", 2000);
}

void MainWindow::cycleScaleMode() {
  switch (m_printSettings.scaleMode) {
  case PrintSettings::FitToPage:
    m_printSettings.scaleMode = PrintSettings::ActualSize;
    break;
  case PrintSettings::ActualSize:
    m_printSettings.scaleMode = PrintSettings::FitToPage;
    break;
  case PrintSettings::CustomPercent:
    // Not implementing custom percent UI yet
    m_printSettings.scaleMode = PrintSettings::FitToPage;
    break;
  }

  for (PageWidget *widget : m_pageWidgets)
    widget->update();

  statusBar()->showMessage(
      QString("Scale: %1").arg(m_printSettings.scaleModeName()), 2000);
}
