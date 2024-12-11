#include "persistent_organic_pollutants_page.hpp"
#include "location_dataset.hpp"
#include "pollutant_card.hpp"
#include <QDebug>
#include <QtCharts>
#include <QtWidgets>

PersistentOrganicPollutantsPage::PersistentOrganicPollutantsPage() : QWidget() {
  setupUI();

  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &PersistentOrganicPollutantsPage::updateChart);
}

void PersistentOrganicPollutantsPage::setupUI() {
  mainLayout = new QVBoxLayout();
  scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);

  centralWidget = new QWidget();
  contentLayout = new QVBoxLayout();

  QLabel *title = new QLabel();
  title->setText(tr("Persistent Organic Pollutants"));
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet(
      "padding: 0px; margin: 0px; font-size: 16px; font-weight: bold;");
  contentLayout->addWidget(title);

  filterDropdown = new QComboBox();
  filterDropdown->addItems({tr("All"), tr("HCH"), tr("PCB"), tr("PBDE")});
  contentLayout->addWidget(filterDropdown);

  connect(filterDropdown, &QComboBox::currentTextChanged, this,
          &PersistentOrganicPollutantsPage::filterCards);
  connect(filterDropdown, &QComboBox::currentTextChanged, this,
          &PersistentOrganicPollutantsPage::updateChart);

  chart = new QChart();
  chart->setTitle(tr("POP Levels Over Time"));
  chart->legend()->setShowToolTips(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  axisX = new QBarCategoryAxis();
  axisX->setTitleText(tr("Pollutant"));

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
      "padding: 0px; margin: 0px; font-size: 14px; font-weight: bold;");
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

void PersistentOrganicPollutantsPage::updateChart() {
  chart->removeAllSeries();
  barSetMap.clear();

  QString selectedFilter = filterDropdown->currentText();
  QMap<QString, QVector<double>> pollutantData;

  const auto &locationDataset = LocationDataset::instance().data;

  for (const Sample &sample : locationDataset) {
    QString determinandLabel =
        QString::fromStdString(sample.getDeterminand().getLabel());

    if (!determinandsMap.contains(determinandLabel)) {
      continue;
    }

    if (selectedFilter != tr("All")) {
      if (selectedFilter == tr("HCH") &&
          !determinandLabel.startsWith("HCH", Qt::CaseInsensitive)) {
        continue;
      } else if (selectedFilter == tr("PCB") &&
                 !determinandLabel.startsWith("PCB", Qt::CaseInsensitive)) {
        continue;
      } else if (selectedFilter == tr("PBDE") &&
                 !determinandLabel.startsWith("PBDE", Qt::CaseInsensitive)) {
        continue;
      }
    }

    if (!pollutantData.contains(determinandLabel)) {
      pollutantData[determinandLabel] = QVector<double>();
    }

    pollutantData[determinandLabel].append(sample.getResult().getValue());
  }

  QBarSeries *barSeries = new QBarSeries();

  for (auto it = pollutantData.begin(); it != pollutantData.end(); ++it) {
    const QString &determinand = it.key();
    const QVector<double> &values = it.value();

    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double average = values.isEmpty() ? 0.0 : sum / values.size();

    QBarSet *barSet = new QBarSet(determinand);
    barSet->append(average);

    barSeries->append(barSet);
  }

  chart->addSeries(barSeries);

  double minValue = 0.0;
  double maxValue = 0.0;
  for (QBarSet *barSet : barSeries->barSets()) {
    for (int i = 0; i < barSet->count(); ++i) {
      maxValue = std::max(maxValue, barSet->at(i));
    }
  }
  axisY->setRange(minValue, maxValue);
  chart->addAxis(axisY, Qt::AlignLeft);
  barSeries->attachAxis(axisY);
}

void PersistentOrganicPollutantsPage::filterCards() {
  QString selectedFilter = filterDropdown->currentText();

  for (PollutantCard *card : pollutantCards) {
    QString pollutantName = QString::fromStdString(card->getDeterminandLabel());

    if (selectedFilter == tr("All")) {
      card->setVisible(true);
    } else if (selectedFilter == tr("HCH")) {
      card->setVisible(pollutantName.startsWith("HCH", Qt::CaseInsensitive));
    } else if (selectedFilter == tr("PCB")) {
      card->setVisible(pollutantName.startsWith("PCB", Qt::CaseInsensitive));
    } else if (selectedFilter == tr("PBDE")) {
      card->setVisible(pollutantName.startsWith("PBDE", Qt::CaseInsensitive));
    } else {
      card->hide();
    }
  }
}
