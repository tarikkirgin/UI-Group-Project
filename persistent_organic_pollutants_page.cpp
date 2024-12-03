#include "persistent_organic_pollutants_page.hpp"
#include "location_dataset.hpp"
#include <QDebug>
#include <QTWidgets>
#include <QtCharts>

PersistentOrganicPollutantsPage::PersistentOrganicPollutantsPage() : QWidget() {
  setupUI();

  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &PersistentOrganicPollutantsPage::updateChart);
}

void PersistentOrganicPollutantsPage::setupUI() {}

void PersistentOrganicPollutantsPage::updateChart() {}
