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

  QScrollArea *scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  QWidget *centralWidget = new QWidget();
  QVBoxLayout *contentLayout = new QVBoxLayout();

  searchBar = new QLineEdit();
  searchBar->setPlaceholderText("Type to filter pollutants...");
  connect(searchBar, &QLineEdit::textChanged, this,
          &PollutantOverviewPage::filterCards);
  contentLayout->addWidget(searchBar);

  series = new QLineSeries();
  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Pollutant Levels Over Time");
  chart->createDefaultAxes();

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  QWidget *cardContainer = new QWidget();
  FlowLayout *flowLayout = new FlowLayout(cardContainer, -1, 20, 20);

  for (auto pollutant = determinandsMap.begin();
       pollutant != determinandsMap.end(); pollutant++) {
    PollutantCard *pollutant_card =
        new PollutantCard(pollutant.key().toStdString(), pollutant.value());
    pollutant_card->setMaximumWidth(350);
    flowLayout->addWidget(pollutant_card);
    pollutantCards.push_back(pollutant_card);
  }

  contentLayout->addWidget(chartView);
  contentLayout->addWidget(cardContainer);

  centralWidget->setLayout(contentLayout);

  scrollArea->setWidget(centralWidget);

  mainLayout->addWidget(scrollArea);

  setLayout(mainLayout);

  updateChart();
}

void PollutantOverviewPage::filterCards() {
  QString searchText = searchBar->text().toLower();

  for (PollutantCard *card : pollutantCards) {
    if (QString::fromStdString(card->getDeterminandLabel())
            .toLower()
            .contains(searchText)) {
      card->show();
    } else {
      card->hide();
    }
  }

  updateChart();
}

void PollutantOverviewPage::updateChart() {
  QChart *chart = chartView->chart();
  chart->removeAllSeries();

  const auto &locationDataset = LocationDataset::instance().data;

  QMap<QString, QLineSeries *> seriesMap;

  QString searchText = searchBar->text().toLower();

  for (int i = 0; i < locationDataset.size(); ++i) {
    const Sample &sample = locationDataset[i];
    QString determinandLabel =
        QString::fromStdString(sample.getDeterminand().getLabel());

    if (!determinandsMap.contains(determinandLabel) ||
        !determinandLabel.toLower().contains(searchText)) {
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
