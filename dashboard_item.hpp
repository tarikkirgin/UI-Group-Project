#pragma once

#include <QGroupBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

class DashboardItem : public QGroupBox {
  Q_OBJECT

public:
  DashboardItem(const QString &labelText, int pageIndex);

signals:
  void navigateToPage(int pageIndex);

private:
  void createMainWidget(const QString &labelText);

  int pageIndex;

  QVBoxLayout *layout;
  QString *name;
  QPushButton *button;
};
