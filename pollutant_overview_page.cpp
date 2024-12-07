#include "pollutant_overview_page.hpp"
#include "location_dataset.hpp"
#include <QDebug>
#include <QtCharts>
#include <QtWidgets>

PollutantOverviewPage::PollutantOverviewPage() : QWidget() {
  setupUI();

  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &PollutantOverviewPage::updateChart);
}

void PollutantOverviewPage::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  series = new QLineSeries();
  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Pollutant Levels Over Time");
  chart->createDefaultAxes();

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  pcard = new PollutantCard();

  mainLayout->addWidget(chartView);
  mainLayout->addWidget(pcard);

  setLayout(mainLayout);

  updateChart();
}

void PollutantOverviewPage::updateChart() {
  QChart *chart = chartView->chart();
  chart->removeAllSeries();

  const auto &locationDataset = LocationDataset::instance().data;

  QSet<QString> allowedDeterminands = {"Chloroform", "112TCEthan", "Atrazine"};
  QMap<QString, QLineSeries *> seriesMap;

  for (int i = 0; i < locationDataset.size(); ++i) {
    const Sample &sample = locationDataset[i];
    QString determinandLabel =
        QString::fromStdString(sample.getDeterminand().getLabel());

    if (!allowedDeterminands.contains(determinandLabel)) {
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
