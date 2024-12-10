#pragma once
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>

class PollutantCard : public QGroupBox {
  Q_OBJECT

public:
  PollutantCard(const std::string &determinandLabel, double complianceLevel);
  std::string getDeterminandLabel() const { return determinandLabel; }

private slots:
  void updateUI();

private:
  void setupUI();
  std::string determinandLabel;
  double complianceLevel;
  QLabel *valueLabel;
  QLabel *complianceLabel;
};
