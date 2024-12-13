#include "pollutant_card.hpp"
#include "location_dataset.hpp"
#include <QVBoxLayout>
// constructor
PollutantCard::PollutantCard(const std::string &determinandLabel,
                             double complianceLevel)
    : QGroupBox(QString::fromStdString(determinandLabel)),
      determinandLabel(determinandLabel), complianceLevel(complianceLevel) {
  setupUI();

  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &PollutantCard::updateUI);
}

void PollutantCard::setupUI() {
  auto *layout = new QVBoxLayout();

  valueLabel = new QLabel();
  complianceLabel = new QLabel();

  layout->addWidget(valueLabel);
  layout->addWidget(complianceLabel);

  setLayout(layout);
}

void PollutantCard::updateUI() {
  const auto &locationDataset = LocationDataset::instance().data;

  std::string unit;
  double total = 0.0;
  int count = 0;

  for (const auto &sample : locationDataset) {
    if (sample.getDeterminand().getLabel() == determinandLabel) {
      unit = sample.getDeterminand().getUnit();
      total += sample.getResult().getValue();
      count++;
    }
  }
  if (count > 0) {
    double valueLevel = total / count;
    valueLabel->setText(QString(tr("Average Value: %1 %2"))
                            .arg(valueLevel)
                            .arg(QString::fromStdString(unit)));

    complianceLabel->setText(QString(tr("Compliance Level: Under %1 %2"))
                                 .arg(complianceLevel)
                                 .arg(QString::fromStdString(unit)));
    if (valueLevel > complianceLevel) {
      setProperty("complianceLevel", "low");
    } else if (valueLevel == complianceLevel) {
      setProperty("complianceLevel", "medium");
    } else {
      setProperty("complianceLevel", "high");
    }
  } else {
    valueLabel->setText(tr("Average Value: N/A"));
    setProperty("complianceLevel", "");
  }
  style()->unpolish(this);
  style()->polish(this);
}
