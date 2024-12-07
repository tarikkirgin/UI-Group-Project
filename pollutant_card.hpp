#pragma once
#include <QGroupBox>
#include <QLabel>

class PollutantCard : public QGroupBox {
  Q_OBJECT

public:
  PollutantCard();

  QLabel *testLabel;

private:
  void setupUI();
  void updateUI();
};
