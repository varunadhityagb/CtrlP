#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Document.h"
#include "PageWidget.h"
#include "PrintSettings.h"
#include <QColor>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  bool loadDocument(const QString &filePath);
  const PrintSettings &printSettings() const { return m_printSettings; }

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  void setupUI();
  void updateStatusBar();
  void renderAllPages();

  void scrollBy(int pixels);
  void jumpToPage(int pageNumber);
  void zoomIn();
  void zoomOut();
  void fitToWidth();
  void fitToHeight();
  int getCurrentVisiblePage();

  void handleNumberKey(int digit);
  void enterCommandMode();
  void exitCommandMode();
  void executeCommand(const QString &cmd);
  void resetKeySequence();

  void cycleMargniPreset();
  void cycleDuplexMode();
  void toggleColorMode();
  void cycleScaleMode();

  enum InputState { NORMAL, AWAITING_G, COMMAND_MODE };

  Document m_document;
  int m_currentPage;
  double m_dpi;

  int m_pageGap;
  int m_scrollAmount;
  bool m_showPageBoundaries;
  QColor m_pageBoundaryColor;

  QScrollArea *m_scrollArea;
  QWidget *m_contentWidget;
  QVBoxLayout *m_contentLayout;
  QList<PageWidget *> m_pageWidgets;

  InputState m_InputState;
  QString m_numberBuffer;
  QTimer *m_keySequenceTimer;
  QLineEdit *m_commandInput;

  PrintSettings m_printSettings;
};

#endif // MAINWINDOW_H_
