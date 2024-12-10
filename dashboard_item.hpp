#pragma once

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

class DashboardItem : public QGroupBox {
  Q_OBJECT

public:
  DashboardItem(const QString &pageTitle, const QString &pageDescription,
                int pageIndex);

signals:
  void navigateToPage(int pageIndex);

private:
  void createMainWidget(const QString &pageDescription);

  int pageIndex;

  QVBoxLayout *layout;
  QString *name;
  QPushButton *button;
  QLabel *descriptionLabel;
};
