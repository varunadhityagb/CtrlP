#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QStringList>
#include <iostream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QStringList args = app.arguments();

  QString filePath;
  if (args.size() > 1)
    filePath = args.at(1);

  MainWindow window;

  if (!filePath.isEmpty()) {
    if (!window.loadDocument(filePath)) {
      QMessageBox::critical(nullptr, "Error", "Failed to load: " + filePath);
      return 1;
    }
  }

  window.show();

  int exitCode = app.exec();

  if (exitCode != 0)
    std::cout << "Application exited with exit code " << exitCode << '\n';

  return exitCode;
}
