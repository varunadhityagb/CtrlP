#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Document.h"
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
  QList<QLabel *> m_pageLabels;

  InputState m_InputState;
  QString m_numberBuffer;
  QTimer *m_keySequenceTimer;
  QLineEdit *m_commandInput;
};

#endif // MAINWINDOW_H_
