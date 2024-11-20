#include "window.hpp"
#include <QtWidgets>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  TestWindow window;
  window.show();

  return app.exec();
}
