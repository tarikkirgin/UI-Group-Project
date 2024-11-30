#pragma once

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class Dashboard : public QWidget {
  Q_OBJECT

public:
  Dashboard();

private:
  void createMainWidget();
  QVBoxLayout *layout;

signals:
  void navigateToPage(int pageIndex);
};
