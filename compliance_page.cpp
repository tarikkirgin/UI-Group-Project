#include "compliance_page.hpp"
#include "location_dataset.hpp"
#include "pollutant_card.hpp"
#include "pollutant_overview_page.hpp"
#include <QDebug>
#include <QtCharts>
#include <QtWidgets>

compliancePage::compliancePage() : QWidget() {
  setupUI();

  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &compliancePage::updateChart);
  connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this,
          &compliancePage::updateCards);
}

void compliancePage::setupUI() {
  mainLayout = new QVBoxLayout();
  scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);

  centralWidget = new QWidget();
  contentLayout = new QVBoxLayout();

  QLabel *title = new QLabel();
  title->setText(tr("Pollutant Compliance Overview"));
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet("padding: 0px; margin: 0px; font-size: 16px; "
                       "font-weight: bold;");
  contentLayout->addWidget(title);

  searchBar = new QLineEdit();
  searchBar->setMaximumWidth(200);
  searchBar->setPlaceholderText(tr("Type to filter pollutants..."));
  connect(searchBar, &QLineEdit::textChanged, this,
          &compliancePage::updateChart);
  connect(searchBar, &QLineEdit::textChanged, this,
          &compliancePage::filterCards);
  contentLayout->addWidget(searchBar);

  complianceChart = new QChart();
  complianceChart->setTitle(tr("Compliance Overview"));
  compliancePieSeries = new QPieSeries();
  complianceChart->addSeries(compliancePieSeries);

  complianceChartView = new QChartView(complianceChart);
  complianceChartView->setRenderHint(QPainter::Antialiasing);
  complianceChartView->setMinimumWidth(400);
  complianceChartView->setMinimumHeight(400);
  contentLayout->addWidget(complianceChartView);

  QLabel *pollutantCardsTitle = new QLabel();
  pollutantCardsTitle->setText(tr("Pollutant levels"));
  pollutantCardsTitle->setStyleSheet(
      "padding: 0px; margin: 0px; font-size: 14px; "
      "font-weight: bold;");
  contentLayout->addWidget(pollutantCardsTitle);

  cardContainer = new QWidget();
  contentLayout->addWidget(cardContainer);
  flowLayout = new FlowLayout(cardContainer, -1, 20, 20);

  centralWidget->setLayout(contentLayout);

  scrollArea->setWidget(centralWidget);

  mainLayout->addWidget(scrollArea);

  setLayout(mainLayout);

  updateChart();
}

void compliancePage::updateCards() {
  pollutantCardsMap.clear();

  const auto &locationDataset = LocationDataset::instance().data;
  for (const auto &sample : locationDataset) {
    QString determinandLabel =
        QString::fromStdString(sample.getDeterminand().getLabel());

    if (!pollutantCardsMap.contains(determinandLabel)) {
      PollutantCard *pollutant_card =
          new PollutantCard(determinandLabel.toStdString(), 1.0);
      pollutantCardsMap[determinandLabel] = pollutant_card;
      pollutant_card->setFixedSize(250, 100);
      flowLayout->addWidget(pollutant_card);
    }
  }
}

void compliancePage::filterCards() {
  QString searchText = searchBar->text().toLower();

  for (auto it = pollutantCardsMap.begin(); it != pollutantCardsMap.end();
       ++it) {
    PollutantCard *card = it.value();
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

void compliancePage::updateChart() {
  compliancePieSeries->clear();

  const auto &locationDataset = LocationDataset::instance().data;
  QString searchText = searchBar->text().toLower();

  int compliantCount = 0;
  int nonCompliantCount = 0;

  for (const auto &sample : locationDataset) {
    QString determinandLabel =
        QString::fromStdString(sample.getDeterminand().getLabel());

    if (!determinandLabel.toLower().contains(searchText)) {
      continue;
    }

    if (sample.getResult().getValue() <= 1) {
      compliantCount++;
    } else {
      nonCompliantCount++;
    }
  }

  QPieSlice *compliantSlice =
      compliancePieSeries->append(tr("Compliant"), compliantCount);
  QPieSlice *nonCompliantSlice =
      compliancePieSeries->append(tr("Non-Compliant"), nonCompliantCount);

  int totalCount = compliantCount + nonCompliantCount;
  double compliantPercentage =
      totalCount > 0 ? (static_cast<double>(compliantCount) / totalCount) * 100
                     : 0;
  double nonCompliantPercentage =
      totalCount > 0
          ? (static_cast<double>(nonCompliantCount) / totalCount) * 100
          : 0;

  compliantSlice->setLabel(
      tr("Compliant: %1 (%2\%)").arg(compliantCount).arg(compliantPercentage));
  nonCompliantSlice->setLabel(tr("Non-Compliant: %1 (%2\%)")
                                  .arg(nonCompliantCount)
                                  .arg(nonCompliantPercentage));
}
