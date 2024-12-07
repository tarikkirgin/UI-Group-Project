#include "pollutant_card.hpp"
#include "location_dataset.hpp"
#include <QtWidgets>

PollutantCard::PollutantCard() : QGroupBox("test") {
  setupUI();

  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &PollutantCard::updateUI);
}

void PollutantCard::setupUI() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  testLabel = new QLabel("Initial Text");
  layout->addWidget(testLabel);
  setLayout(layout);
}

void PollutantCard::updateUI() { testLabel->setText("hey"); }
