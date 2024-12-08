#pragma once
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

class PollutantCard : public QGroupBox {
  Q_OBJECT

public:
  PollutantCard(const std::string &determinandLabel, double complianceLevel);

private slots:
  void updateUI();

private:
  void setupUI();
  std::string determinandLabel;
  double complianceLevel;
  QLabel *valueLabel;
  QLabel *complianceLabel;
};
