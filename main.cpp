#include "window.hpp"
#include <QtWidgets>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QFile file("../style.qss");
  if (file.open(QFile::ReadOnly)) {
    QString StyleSheet = QLatin1String(file.readAll());
    app.setStyleSheet(StyleSheet);
  }

  QTranslator trn;
  if (trn.load(QLocale::system(), "watertool", "_", ":/i18n")) {
    app.installTranslator(&trn);
  }

  Window window;
  window.show();

  return app.exec();
}
