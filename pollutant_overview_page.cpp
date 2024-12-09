#include "pollutant_overview_page.hpp"
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
  mainLayout = new QVBoxLayout();
  scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);

  centralWidget = new QWidget();
  contentLayout = new QVBoxLayout();

  searchBar = new QLineEdit();
  searchBar->setPlaceholderText("Type to filter pollutants...");
  connect(searchBar, &QLineEdit::textChanged, this,
          &PollutantOverviewPage::filterCards);
  contentLayout->addWidget(searchBar);

  chart = new QChart();
  chart->setTitle("Pollutant Levels Over Time");

  axisX = new QDateTimeAxis();
  axisX->setFormat("MMM yyyy");
  axisX->setTitleText("Date");

  axisY = new QValueAxis();
  axisY->setTitleText("Value ug/l");

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setMinimumWidth(800);
  chartView->setFixedHeight(400);
  chartView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  chartScrollArea = new QScrollArea();
  chartScrollArea->setWidgetResizable(false);
  chartScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  chartScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  chartScrollArea->setWidget(chartView);

  cardContainer = new QWidget();
  flowLayout = new FlowLayout(cardContainer, -1, 20, 20);

  for (auto pollutant = determinandsMap.begin();
       pollutant != determinandsMap.end(); pollutant++) {
    PollutantCard *pollutant_card =
        new PollutantCard(pollutant.key().toStdString(), pollutant.value());
    pollutant_card->setFixedSize(250, 100);
    flowLayout->addWidget(pollutant_card);
    pollutantCards.push_back(pollutant_card);
  }

  contentLayout->addWidget(chartScrollArea);
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
      newSeries->attachAxis(axisX);
      newSeries->attachAxis(axisY);
    }

    double time = QDateTime::fromString(
                      QString::fromStdString(sample.getTime()), Qt::ISODate)
                      .toMSecsSinceEpoch();
    double value = sample.getResult().getValue();
    seriesMap[determinandLabel]->append(time, value);
  }

  if (locationDataset.size() > 0) {
    auto firstSampleTime = QDateTime::fromString(
        QString::fromStdString(locationDataset[0].getTime()), Qt::ISODate);
    auto lastSampleTime = QDateTime::fromString(
        QString::fromStdString(
            locationDataset[locationDataset.size() - 1].getTime()),
        Qt::ISODate);

    axisX->setRange(firstSampleTime, lastSampleTime);
    chart->addAxis(axisX, Qt::AlignBottom);

    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();
    for (auto series : seriesMap) {
      for (const QPointF &point : series->points()) {
        minValue = std::min(minValue, point.y());
        maxValue = std::max(maxValue, point.y());
      }
    }
    axisY->setRange(minValue, maxValue);
    chart->addAxis(axisY, Qt::AlignLeft);
  }
}
