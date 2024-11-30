#pragma once

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class Page : public QWidget {
  Q_OBJECT

public:
  Page(const QString &title);

signals:
  void navigateBack();

protected:
  QVBoxLayout *layout;
  QLabel *titleLabel;
  QPushButton *backButton;

  void setupUI(const QString &title);
};
