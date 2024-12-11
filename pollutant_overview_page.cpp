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
  scrollArea->setFrameShape(QFrame::NoFrame);

  centralWidget = new QWidget();
  contentLayout = new QVBoxLayout();

  QLabel *title = new QLabel();
  title->setText(tr("Pollutant Overview Page"));
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet("padding: 0px; margin: 0px; font-size: 16px; "
                       "font-weight: bold;");
  contentLayout->addWidget(title);

  searchBar = new QLineEdit();
  searchBar->setMaximumWidth(200);
  searchBar->setPlaceholderText(tr("Type to filter pollutants..."));
  connect(searchBar, &QLineEdit::textChanged, this,
          &PollutantOverviewPage::filterCards);
  contentLayout->addWidget(searchBar);

  chart = new QChart();
  chart->setTitle(tr("Pollutant Levels Over Time"));
  chart->legend()->setShowToolTips(true);

  axisX = new QDateTimeAxis();
  axisX->setFormat("MMM yyyy");
  axisX->setTitleText(tr("Date"));

  axisY = new QValueAxis();
  axisY->setTitleText(tr("Concentration (ug/l)"));

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setMinimumWidth(400);
  chartView->setMinimumHeight(400);

  chartScrollArea = new QScrollArea();
  chartScrollArea->setWidgetResizable(true);
  chartScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  chartScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  chartScrollArea->setWidget(chartView);
  chartScrollArea->setMinimumHeight(400);
  chartScrollArea->setFrameShape(QFrame::NoFrame);
  contentLayout->addWidget(chartScrollArea);

  QLabel *pollutantCardsTitle = new QLabel();
  pollutantCardsTitle->setText(tr("Pollutant levels"));
  pollutantCardsTitle->setStyleSheet(
      "padding: 0px; margin: 0px; font-size: 14px; "
      "font-weight: bold;");
  contentLayout->addWidget(pollutantCardsTitle);

  cardContainer = new QWidget();
  contentLayout->addWidget(cardContainer);
  flowLayout = new FlowLayout(cardContainer, -1, 20, 20);

  for (auto pollutant = determinandsMap.begin();
       pollutant != determinandsMap.end(); pollutant++) {
    PollutantCard *pollutant_card =
        new PollutantCard(pollutant.key().toStdString(), 0.5);
    pollutant_card->setFixedSize(250, 100);
    flowLayout->addWidget(pollutant_card);
    pollutantCards.push_back(pollutant_card);
  }

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
  seriesMap.clear();

  const auto &locationDataset = LocationDataset::instance().data;
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
      newSeries->setPointsVisible(true);
      connect(newSeries, &QLineSeries::hovered, this,
              &PollutantOverviewPage::onPointHovered);
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

void PollutantOverviewPage::onPointHovered(const QPointF &point, bool state) {
  // cast sent pointer to QLineSeries*
  QLineSeries *series = qobject_cast<QLineSeries *>(sender());
  if (!series)
    return;

  QString determinandName;
  for (auto it = seriesMap.begin(); it != seriesMap.end(); ++it) {
    if (it.value() == series) {
      determinandName = it.key();
      break;
    }
  }

  if (state) { // hover starts
    QString tooltipText =
        QString("Determinand: %1\nValue: %2\nTime: %3")
            .arg(determinandName)
            .arg(point.y())
            .arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString(
                "yyyy-MM-dd hh:mm:ss"));
    QToolTip::showText(QCursor::pos(), tooltipText, chartView);
  } else { // hover ends
    QToolTip::hideText();
  }
}
