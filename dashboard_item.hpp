#pragma once

#include <QGroupBox>

class QLabel;
class QVBoxLayout;
class QPushButton;

class DashboardItem : public QGroupBox {
  Q_OBJECT

public:
  DashboardItem(const QString &labelText = "");

private:
  void createMainWidget(const QString &labelText);

  QVBoxLayout *layout;
  QLabel *name;
  QPushButton *button;
};
