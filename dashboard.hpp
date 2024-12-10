#pragma once
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class Dashboard : public QWidget {
  Q_OBJECT

public:
  Dashboard(const QMap<QString, QString> &pageDetails);

private:
  void createMainWidget();
  QVBoxLayout *layout;
  QLabel *footerLabel;
  QLabel *title;
  QMap<QString, QString> pageDetails;

signals:
  void navigateToPage(int pageIndex);
};
