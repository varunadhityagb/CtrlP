#include "MainWindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  MainWindow window;
  window.show();

  int exitCode = app.exec();

  if (exitCode != 0)
    std::cout << "Application exited with exit code " << exitCode << '\n';

  return exitCode;
}
