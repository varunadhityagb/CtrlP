#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Document.h"
#include <QColor>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
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
};

#endif // MAINWINDOW_H_
