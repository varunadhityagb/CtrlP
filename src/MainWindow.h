#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    void createMenus();
    
};

#endif // MAINWINDOW_H_
