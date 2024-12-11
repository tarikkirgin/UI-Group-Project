#include "compliance_page.hpp"
#include "pollutant_dataset.hpp"
#include "location_dataset.hpp"
#include "dataset.hpp"
#include "sample.hpp"
#include <QDebug>
#include <QtWidgets>
#include <QtCharts>
#include <QString>
#include <QColor>


 compliancePage::compliancePage() : QWidget() {
    // qDebug() << "A";
    setupUI();
    // qDebug() << "B";
    connect(pollutantComboBox, &QComboBox::currentTextChanged, //takes the string value of text that's changed
    &PollutantDataset::instance(), &PollutantDataset::onPollutantChanged); // => this will change the value of PollutantDataset::instance().data
    // qDebug() << "C"; //send the string value to onPollutantChanged();
    connect(complianceComboBox, &QComboBox::currentTextChanged,
    &PollutantDataset::instance(), &PollutantDataset::onComplianceChanged); // => this will change the value of PollutantDataset::instance().data
    // qDebug() << "D";
    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this, &compliancePage::updateDropdownChart);
    // qDebug() << "E"; 
    connect(&PollutantDataset::instance(), &PollutantDataset::dataUpdated,this, &compliancePage::getCardUI);
    connect(&PollutantDataset::instance(), &PollutantDataset::dataUpdated,this, &compliancePage::getCardUI);
 }

void compliancePage::setupUI() {
  mainWithScrollLayout = new QHBoxLayout();
  mainLayout = new QVBoxLayout();
  //for titles!======================================================================
  QHBoxLayout *titleLayout = new QHBoxLayout();
  titleLayout->setContentsMargins(0, 0, 0, 0);
  QLabel *title = new QLabel();
  title->setText("Compliance Page");
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet("padding: 0px; margin: 0px; font-size: 16px; font-weight: bold; ");
  QFontMetrics fm(title->font());
  int textHeight = fm.height();
  title->setMinimumHeight(textHeight);
  title->setMaximumHeight(textHeight);
  titleLayout->addWidget(title);
  mainLayout->addLayout(titleLayout);
  //for dropdowns!====================================================================
  QHBoxLayout *dropdown = new QHBoxLayout();
  pollutantComboBox = new QComboBox();
  pollutantComboBox->setEditable(true);
  pollutantComboBox->setMinimumWidth(150);
  dropdown->addWidget(pollutantComboBox);
  complianceComboBox = new QComboBox();
  complianceComboBox->setEditable(true);
  complianceComboBox->setMinimumWidth(150);
  QStringList complianceList = {"MAX","High","Medium","Low","min"};
  complianceComboBox->addItems(complianceList);
  dropdown->addWidget(complianceComboBox);
  mainLayout->addLayout(dropdown);
  //for charts!=========================================================================================
  series = new QBarSeries();
  chart = new QChart();
  chart->addSeries(series);
  axisX = new QBarCategoryAxis(); 
  // series->attachAxis(axisX);
  chart->addAxis(axisX, Qt::AlignBottom);
  axisY = new QValueAxis();
  axisY->setTitleText("Measurements");
  chart->addAxis(axisY, Qt::AlignLeft);
  // series->attachAxis(axisY);
  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setMinimumHeight(350);;  
  mainLayout->addWidget(chartView);
  //for cards! =========================================================================================
  // cardsLayout = new QGridLayout();
  // cards = new QWidget();
  // cards->setLayout(cardsLayout);
  cardContainer = new QWidget();
  mainLayout->addWidget(cardContainer);
  //for scrolls!========================================================================================
  QScrollArea *scrollArea = new QScrollArea();
  QWidget *contentWidget = new QWidget(); // The widget you want to scroll
  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);  // Ensure resizing works
  mainWithScrollLayout->addLayout(mainLayout);
  mainWithScrollLayout->addWidget(scrollArea, 0);
  scrollArea->setFixedWidth(200);
  setLayout(mainWithScrollLayout);
}

void compliancePage::updateDropdownChart() { 
    // making new list for Pollutant Dropdowns!!=======================================================
    QStringList pollutantList;
    for (const auto &pollutant : LocationDataset::instance().data) {
      pollutantList << QString::fromStdString(pollutant.getDeterminand().getLabel());
    }
    pollutantComboBox->clear();
    pollutantComboBox->addItems(pollutantList); //we added pollutant dropdown!!
    QCompleter *completer = new QCompleter(pollutantList, pollutantComboBox);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    pollutantComboBox->setCompleter(completer);
    // Cleaning up all the series in the chart!!==========================================================
    chart->removeSeries(series);
    delete series;
    series = new QBarSeries();
    chart->addSeries(series);
    // make new chart barset that will go to new Series===================================================
    chart->setTitle(QString::asprintf("Compliance Indicator for %s"
     , LocationDataset::instance().data[0].getSamplingPoint().getLabel().c_str()));
    
    getminMAX(LocationDataset::instance().data);
    axisY->setRange(min,MAX);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    // fill up the barsets=================================================================================
    barSets.clear();
    int i = 0;
    Determinand temp = LocationDataset::instance().data[0].getDeterminand();  
    barSets.push_back(new QBarSet(QString::fromStdString(LocationDataset::instance().data[0].getDeterminand().getLabel())));
    for (const auto &sampleData : LocationDataset::instance().data) {
        if(sampleData.getDeterminand().getLabel() != temp.getLabel()) {
            barSets.push_back(new QBarSet(QString::fromStdString(temp.getLabel())));
            qDebug() << "newLabelDected"; 
            series->append(barSets[i]);
            PollutantDataset::instance().onPollutantChanged(QString::fromStdString(temp.getLabel()));
            getAverage();
            if (getLevel(sampleData) == "min") {
                barSets[i]->setColor(QColor(0, 100, 0));
              } else if (getLevel(sampleData) == "MAX") {
                barSets[i]->setColor(QColor(139, 0, 0));
              } else if (getLevel(sampleData) == "high") {
                  barSets[i]->setColor(Qt::red);
              } else if (getLevel(sampleData) == "medium") {
                  barSets[i]->setColor(QColor("#FFBF00"));
              } else {
                  barSets[i]->setColor(Qt::green); 
              }
            *barSets[++i] << sampleData.getResult().getValue();
            temp = sampleData.getDeterminand();
        } else {
            *barSets[i] << sampleData.getResult().getValue();
            qDebug() << "addingOnToExistingLabel"; 
        }
    }
    chartView->show();          
}

void compliancePage::getCardUI() { // MAKE CHART DISAPPEAR TOO
  //  getAverage();
  // const int maxColumns = 3; // Maximum columns per row
  // int currentIndex = 0;
  // int row = currentIndex / maxColumns;
  // int col = currentIndex % maxColumns;
  // int i = 0;
  // for(const auto &data : PollutantDataset::instance().data) {
  //   QVBoxLayout *cardLayout = new QVBoxLayout();
  //   QWidget *eachCardContainer = new QWidget();
  //   eachCardContainer->setLayout(cardLayout);
  //   QLabel *valueLabel = new QLabel(QString::asprintf("The measurement has %f%s more than average"
  //   , average - data.getResult().getValue(), data.getDeterminand().getUnit().c_str()));
  //   QLabel *complianceLabel = new QLabel(QString::asprintf("The compliance indicates %s-level state"
  //   , getLevel(data).c_str()));
  //   cardLayout->addWidget(valueLabel);
  //   cardLayout->addWidget(complianceLabel);
  //   cardsLayout->addWidget(eachCardContainer, row, col);
  //   i++;
  //   currentIndex++;
    FlowLayout *flowLayout = new FlowLayout(cardContainer, -1, 20, 20);
   for (const auto &sample : PollutantDataset::instance().data){
        everyPollutantCard *pollutant_card = new everyPollutantCard(sample.getDeterminand().getLabel(), sample.getResult().getValue());
        pollutant_card->setMaximumWidth(350);
        flowLayout->addWidget(pollutant_card);
        // pollutantCards.push_back(pollutant_card);
    }
}

void compliancePage::getAverage() {
  double total = 0.0;
  int count = 0;
  for (const auto &sample : PollutantDataset::instance().data) {
      total += sample.getResult().getValue();
      count++;
    }
  average = total / count;
}

void compliancePage::getminMAX(std::vector<Sample> dataset) {
  Sample temp = dataset[0];
  for (const auto &sample : dataset) {
      if (temp.getResult().getValue() > sample.getResult().getValue()) {
        temp = sample;
      }
  }
  min = temp.getResult().getValue();
  for (const auto &sample : PollutantDataset::instance().data) {
      if (temp.getResult().getValue() < sample.getResult().getValue()) {
        temp = sample;
      }
  }
  MAX = temp.getResult().getValue();
}

std::string compliancePage::getLevel(Sample sample) {
  if (sample.getResult().getValue() == min) {
      return "min";
  } else if (sample.getResult().getValue() == MAX) {
      return "MAX";
  } else if (sample.getResult().getValue() > average - (min + MAX) / 10
   && sample.getResult().getValue() < average + (min + MAX) / 10) {
      return "Medium";
  } else if (sample.getResult().getValue() < average) {
      return "Low";
  } else {
      return "High";
  }
}


  // cardHolder.push_back(new QWidget());
  // cardLayout.push_back(new QVBoxLayout());
  // valueLabel.push_back(new QLabel()); 
  // complianceLabel.push_back(new QLabel()); 
  // cardLayout[i]->addWidget(valueLabel[i]);
  // cardLayout[i]->addWidget(complianceLabel[i]);
  // cardHolder[i]->setLayout(cardLayout[i]);
  // valueLabel[i]->setText(QString::asprintf("The measurement has %f%s more than average", average - data.getResult().getValue(), data.getDeterminand().getUnit()));
  // complianceLabel[i]->setText(QString::asprintf("The compliance indicates %s-level state", getLevel(data)));
  // cards->addWidget(cardHolder[i], row, col);
  // i++;
  // currentIndex++;
  everyPollutantCard::everyPollutantCard(const std::string &determinandLabel,
                             double complianceLevel)
    : QGroupBox(QString::fromStdString(determinandLabel)),
      determinandLabel(determinandLabel), complianceLevel(complianceLevel) {
  setupUI();

  connect(&PollutantDataset::instance(), &PollutantDataset::dataUpdated, this,
          &everyPollutantCard::updateUI);
}

void everyPollutantCard::setupUI() {
  auto *layout = new QVBoxLayout();

  valueLabel = new QLabel();
  complianceLabel = new QLabel();

  layout->addWidget(valueLabel);
  layout->addWidget(complianceLabel);

  setLayout(layout);
  
}


  void everyPollutantCard::updateUI() {
  const auto &PollutantDataset = PollutantDataset::instance().data;

  std::string unit;
  double total = 0.0;
  int count = 0;

  for (const auto &sample : PollutantDataset) {
    if (sample.getDeterminand().getLabel() == determinandLabel) {
      unit = sample.getDeterminand().getUnit();
      total += sample.getResult().getValue();
      count++;
    }
  }
  if (count > 0) {
    double average = total / count;
    valueLabel->setText(QString("Average Value: %1 %2")
                            .arg(average)
                            .arg(QString::fromStdString(unit)));

    complianceLabel->setText(QString("Compliance Level: Under %1 %2")
                                 .arg(complianceLevel)
                                 .arg(QString::fromStdString(unit)));
    if (average > complianceLevel) {
      setProperty("complianceLevel", "low");
    } else if (average == complianceLevel) {
      setProperty("complianceLevel", "medium");
    } else {
      setProperty("complianceLevel", "high");
    }
  } else {
    valueLabel->setText("Average Value: N/A");
  }
  style()->unpolish(this);
  style()->polish(this);
}
