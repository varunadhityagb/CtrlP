#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Document.h"
#include <QKeyEvent>
#include <QMainWindow>

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
  void nextPage();
  void previousPage();

  Document m_document;
  int m_currentPage;
};

#endif // MAINWINDOW_H_
