#include "pollutant_overview_page.hpp"
#include "flowlayout.h"
#include "location_dataset.hpp"
#include "pollutant_card.hpp"
#include <QDebug>
#include <QtCharts>
#include <QtWidgets>

PollutantOverviewPage::PollutantOverviewPage() : QWidget() {
  setupUI();

  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &PollutantOverviewPage::updateChart);
}

void PollutantOverviewPage::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout();

  series = new QLineSeries();
  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Pollutant Levels Over Time");
  chart->createDefaultAxes();

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  QWidget *cardContainer = new QWidget();
  FlowLayout *flowLayout = new FlowLayout(cardContainer, -1, 20, 20);

  // Define the map with determinand names and compliance levels
  QMap<QString, double> determinands = {
      {"Chloroform", 0.75}, {"112TCEthan", 0.60}, {"Atrazine", 0.85}};

  // Loop through the map instead of the set
  for (auto it = determinands.begin(); it != determinands.end(); ++it) {
    // Pass both the determinand name and compliance level to the card
    PollutantCard *pollutant_card =
        new PollutantCard(it.key().toStdString(), it.value());
    pollutant_card->setMaximumWidth(350);
    flowLayout->addWidget(pollutant_card);
  }

  mainLayout->addWidget(chartView);
  mainLayout->addWidget(cardContainer);

  setLayout(mainLayout);

  updateChart();
}

void PollutantOverviewPage::updateChart() {
  QChart *chart = chartView->chart();
  chart->removeAllSeries();

  const auto &locationDataset = LocationDataset::instance().data;

  QMap<QString, QLineSeries *> seriesMap;

  for (int i = 0; i < locationDataset.size(); ++i) {
    const Sample &sample = locationDataset[i];
    QString determinandLabel =
        QString::fromStdString(sample.getDeterminand().getLabel());

    if (!determinands.contains(determinandLabel)) {
      continue;
    }

    if (!seriesMap.contains(determinandLabel)) {
      QLineSeries *newSeries = new QLineSeries();
      newSeries->setName(determinandLabel);
      seriesMap[determinandLabel] = newSeries;
      chart->addSeries(newSeries);
    }

    double time = QDateTime::fromString(
                      QString::fromStdString(sample.getTime()), Qt::ISODate)
                      .toSecsSinceEpoch();
    double value = sample.getResult().getValue();
    seriesMap[determinandLabel]->append(time, value);
  }

  chart->createDefaultAxes();
  chart->axes(Qt::Horizontal).first()->setTitleText("Time");
  chart->axes(Qt::Vertical).first()->setTitleText("Result Value");

  if (locationDataset.size() > 0) {
    auto firstSampleTime =
        QDateTime::fromString(
            QString::fromStdString(locationDataset[0].getTime()), Qt::ISODate)
            .toSecsSinceEpoch();
    auto lastSampleTime =
        QDateTime::fromString(
            QString::fromStdString(
                locationDataset[locationDataset.size() - 1].getTime()),
            Qt::ISODate)
            .toSecsSinceEpoch();
    chart->axes(Qt::Horizontal)
        .first()
        ->setRange(firstSampleTime, lastSampleTime);

    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();
    for (auto series : seriesMap) {
      for (const QPointF &point : series->points()) {
        minValue = std::min(minValue, point.y());
        maxValue = std::max(maxValue, point.y());
      }
    }
    chart->axes(Qt::Vertical).first()->setRange(minValue, maxValue);
  }

  chart->setTitle("Pollutant Levels Over Time");
}
