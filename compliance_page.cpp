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
    &PollutantDataset::instance(), &PollutantDataset::onPollutantChanged);
    // qDebug() << "C"; //send the string value to onPollutantChanged();
    connect(complianceComboBox, &QComboBox::currentTextChanged,
    &PollutantDataset::instance(), &PollutantDataset::onComplianceChanged);
    // qDebug() << "D";
    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this, &compliancePage::updateDropdownChart);
    // qDebug() << "E"; 
    connect(pollutantComboBox, &QComboBox::currentTextChanged,this, &compliancePage::getCardUI);
    connect(complianceComboBox, &QComboBox::currentTextChanged,this, &compliancePage::getCardUI);
 }

void compliancePage::setupUI() {
  //for dropdowns!====================================================================
  mainLayout = new QVBoxLayout();
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
  //for charts!============================================================================
  series = new QBarSeries();
  // series = nullptr;
  chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Measurement/Compliance based on each pollutant");
  chart->createDefaultAxes();
  axisX = new QBarCategoryAxis(); 
  chart->addAxis(axisX, Qt::AlignBottom);
  axisY = new QValueAxis();
  chart->addAxis(axisY, Qt::AlignLeft);
  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  mainLayout->addWidget(chartView);
    //for scrolls!==========================================================================
  scrollArea = new QScrollArea();
  scrollArea->setWidget(new QWidget());         // Embed the content widget
  scrollArea->setWidgetResizable(true);
  mainLayout->addWidget(scrollArea);
  setLayout(mainLayout);
}

void compliancePage::updateDropdownChart() { 
    QStringList pollutantList;
    for (const auto &pollutant : LocationDataset::instance().data) {
      pollutantList << QString::fromStdString(pollutant.getDeterminand().getLabel());
    }
    pollutantComboBox->addItems(pollutantList); //we added pollutant dropdown!!
    // qDebug() << "1"; 
    QCompleter *completer = new QCompleter(pollutantList, pollutantComboBox);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    pollutantComboBox->setCompleter(completer);
    // qDebug() << "1"; 
// if (series != nullptr) {
//     qDebug() << "1"; 
//     chart->removeSeries(series);
//     qDebug() << "1"; 
//     chart->removeAxis(axisX);
//     chart->removeAxis(axisY);
//     delete series;
//     delete axisX;
//     delete axisY;
//     series = new QBarSeries();
//     chart->addSeries(series);
//     } 
    // qDebug() << "1"; 
    chart->removeSeries(series);
    // qDebug() << "1"; 
    // delete series;
    // qDebug() << "1"; 
    series = new QBarSeries();
    qDebug() << "1"; 
    chart->addSeries(series);
    // existingPollutants.clear();
    // make new chart components ======================================================
    std::vector<QBarSet *> barSets;
    chart->removeAxis(axisX);
    delete axisX;
    axisX = new QBarCategoryAxis(); 
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    chart->removeAxis(axisY);
    delete axisY;
    axisY = new QValueAxis(); // allocating axis object to the heap
    axisY->setRange(0,15);
    axisY->setTitleText("Values");
    chart->addAxis(axisY, Qt::AlignLeft);  
    series->attachAxis(axisY); //add to series 
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);
    qDebug() << "2"; 
    // fill up the barsets and axis X==============================================================
    int i = 0;
    Determinand temp = LocationDataset::instance().data[0].getDeterminand();
    // if (LocationDataset::instance().data.empty()) {
    // qDebug() << "Data is empty!";
    // return; // Exit the function or handle the error
    // }
    barSets.push_back(new QBarSet(QString::fromStdString(LocationDataset::instance().data[0].getDeterminand().getLabel())));
    for (const auto &sampleData : LocationDataset::instance().data) {

        if(sampleData.getDeterminand().getLabel() != temp.getLabel()) {
            barSets.push_back(new QBarSet(QString::fromStdString(temp.getLabel())));
            qDebug() << "newLabelDected"; 
            // axisX->append(QString::fromStdString(temp.getLabel()));
            series->append(barSets[i]);
            PollutantDataset::instance().onPollutantChanged(QString::fromStdString(temp.getLabel()));
            getAverage();
            if (getLevel(sampleData) == "min") {
                barSets[i]->setColor(QColor(0, 100, 0));
              } else if (getLevel(sampleData) == "MAX") {
                barSets[i]->setColor(QColor(139, 0, 0));
              } else if (getLevel(sampleData) == "high") {
                  barSets[i]->setColor(Qt::red); // Green for true
              } else if (getLevel(sampleData) == "medium") {
                  barSets[i]->setColor(QColor("#FFBF00"));
              } else {
                  barSets[i]->setColor(Qt::green);   // Red for false
              }
            *barSets[++i] << sampleData.getResult().getValue();
            temp = sampleData.getDeterminand();
        } else {
            *barSets[i] << sampleData.getResult().getValue();
            qDebug() << "addingOnToExistingLabel"; 
        }
    }
    chartView->show();
    qDebug() << "4";           
    ungetCardUI();
}

void compliancePage::getCardUI() { // MAKE CHART DISAPPEAR TOO
  chartView->hide();
  //for cards!===========================================================================
  cardsLayout = new QGridLayout();
  cards = new QWidget();
  cards->setLayout(cardsLayout);
  card();
  mainLayout->addWidget(cards);
  scrollArea->show();
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

void compliancePage::getminMAX() {
  Sample temp = PollutantDataset::instance().data[0];
  for (const auto &sample : PollutantDataset::instance().data) {
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

void compliancePage::ungetCardUI() {
  qDebug() << "4.5"; 
  cards->hide();  
  qDebug() << "5"; 
  // scrollArea->hide();
  qDebug() << "6"; 
}

void compliancePage::card() { //they are all the same pollutant
  getAverage();
  const int maxColumns = 3; // Maximum columns per row
  int currentIndex = 0;
  int row = currentIndex / maxColumns;
  int col = currentIndex % maxColumns;
  int i = 0;
  for(const auto &data : PollutantDataset::instance().data) {
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
    QVBoxLayout *cardLayout = new QVBoxLayout();
    QWidget *eachCardContainer = new QWidget();
    eachCardContainer->setLayout(cardLayout);
    QLabel *valueLabel = new QLabel(QString::asprintf("The measurement has %f%s more than average"
    , average - data.getResult().getValue(), data.getDeterminand().getUnit()));
    QLabel *complianceLabel = new QLabel(QString::asprintf("The compliance indicates %s-level state"
    , getLevel(data)));
    cardLayout->addWidget(valueLabel);
    cardLayout->addWidget(complianceLabel);
    cardsLayout->addWidget(eachCardContainer, row, col);
    i++;
    currentIndex++;
  }
}
