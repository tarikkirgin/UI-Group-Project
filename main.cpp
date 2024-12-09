#include "window.hpp"
#include <QtWidgets>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QFile file("../style.qss");
  if (file.open(QFile::ReadOnly)) {
    QString StyleSheet = QLatin1String(file.readAll());
    app.setStyleSheet(StyleSheet);
  }

  Window window;
  window.show();

  return app.exec();
}
