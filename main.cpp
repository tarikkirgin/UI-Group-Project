#include "dashboard.hpp"
#include <QtWidgets>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Dashboard window;
  window.show();

  return app.exec();
}
