#include "compliance_page.hpp"
#include "pollutant_dataset.hpp"
#include "location_dataset.hpp"
#include "dataset.hpp"
#include "sample.hpp"
#include <QDebug>
#include <QtWidgets>
#include <QtCharts>
#include <QString>



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
    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this, &compliancePage::updateDropdown);
    // qDebug() << "E"; 
    connect(&PollutantDataset::instance(), &PollutantDataset::dataUpdated, this, &compliancePage::updateChartDropdown);
 }

void compliancePage::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout();
  QHBoxLayout *dropdown = new QHBoxLayout();
  //for charts!==============================
  series = new QBarSeries();
  chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Measurement/Compliance based on each pollutant");
  chart->createDefaultAxes();
  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  // barSet = new QBarSet("Pollutants");
  
  //for dropdowns!=============================
  pollutantComboBox = new QComboBox();
  pollutantComboBox->setEditable(true);
  pollutantComboBox->setMinimumWidth(150);
  dropdown->addWidget(pollutantComboBox);

  complianceComboBox = new QComboBox();
  complianceComboBox->setEditable(true);
  complianceComboBox->setMinimumWidth(150);
  dropdown->addWidget(complianceComboBox);

  mainLayout->addLayout(dropdown);
  mainLayout->addWidget(chartView);


  setLayout(mainLayout);

}

  void compliancePage::updateDropdown() { 
  std::vector<std::string> pollutants = LocationDataset::instance().getPollutants();
  QStringList pollutantList; //making string array item for Qt!!
  for (const auto &pollutant : pollutants) {
    pollutantList << QString::fromStdString(pollutant);
  }
  pollutantComboBox->addItems(pollutantList); //we added pollutant dropdown!!

  //now let's add compliance dropdown
  std::vector<bool> compliances = LocationDataset::instance().getCompliances();
  for (bool value : compliances) {
    complianceComboBox->addItem(value ? "true" : "false");
  }

  QCompleter *completer = new QCompleter(pollutantList, pollutantComboBox);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  pollutantComboBox->setCompleter(completer);
}

//   void compliancePage::updateDropdown1() { 
//   //now let's add compliance dropdown
//   std::vector<bool> compliances = PollutantDataset::instance().getCompliances();
//   for (bool value : compliances) {
//     complianceComboBox->addItem(value ? "true" : "false");
//   }
// }


void compliancePage::updateChartDropdown() {

  std::vector<std::string> pollutants = PollutantDataset::instance().getPollutants();
  QStringList pollutantList; //making string array item for Qt!!
  for (const auto &pollutant : pollutants) {
    pollutantList << QString::fromStdString(pollutant);
  }
  pollutantComboBox->addItems(pollutantList); //we added pollutant dropdown!!

  //now let's add compliance dropdown
  std::vector<bool> compliances = PollutantDataset::instance().getCompliances();
  for (bool value : compliances) {
    complianceComboBox->addItem(value ? "true" : "false");
  }

  QCompleter *completer = new QCompleter(pollutantList, pollutantComboBox);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  pollutantComboBox->setCompleter(completer);

   qDebug() << "Update Pchart triggered";

    // for (int i = 0; i < barSet->count(); ++i) {
    // barSet->replace(i, 0); // Replace each value with 0
    // }
    if (series != nullptr) {
    chart->removeSeries(series);
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);
    delete series;
    delete axisX;
    delete axisY;
    }
    existingPollutants.clear();
    // chart->removeSeries(series); 
    // delete series;
    // qDebug() << "0.5"; 
    series = new QBarSeries();
    chart->addSeries(series);
    std::vector<QBarSet *> barSets;
    qDebug() << "1";
    existingPollutants.clear();

    int i = 0;
    for (const auto &sampleData : PollutantDataset::instance().data) {
        if (checkOverlaps(sampleData.getDeterminand().getLabel()) == true) {
        barSets.push_back(new QBarSet(QString::fromStdString(sampleData.getDeterminand().getLabel())));
        *barSets[i] << sampleData.getResult().getValue();
        series->append(barSets[i]); // change this to Qstring
        // Set color based on the boolean flag
        if (sampleData.getIsComplianceSample()) {
            barSets[i]->setColor(Qt::red); // Green for true
        } else {
            barSets[i]->setColor(Qt::green);   // Red for false
        }
        i++;
    }  
    }  

    qDebug() << "2";
    // series = new QBarSeries();
    // chart->addSeries(series);

    // QBarSeries *series = new QBarSeries();

    // series->append(barSet); // bars to series

    qDebug() << "3";
    //make X axis
    axisX = new QBarCategoryAxis(); // allocating axis object to the heap
    for (const auto &pollutants : existingPollutants) {
      axisX->append(QString::fromStdString(pollutants));
      qDebug() << QString::fromStdString(pollutants);
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX); // add to series
    //make Y axis
    axisY = new QValueAxis(); // allocating axis object to the heap
    axisY->setRange(0,15);
    axisY->setTitleText("Values");
    chart->addAxis(axisY, Qt::AlignLeft);  
    series->attachAxis(axisY); //add to series

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    // createDefaultChartView(chart);
    qDebug() << "4";
 // all the series to chart

}

bool compliancePage::checkOverlaps(std::string pollutantName) {

  for (const auto pollutants : existingPollutants) {
    if (pollutantName == pollutants) {
      return false;
    } 
  }
  existingPollutants.push_back(pollutantName); 
  return true;
}
 
// void compliancePage::LupdateChart() {
//      qDebug() << "Update Lchart triggered";

//   // const auto &LocationDataset = LocationDataset::instance().data;
//   makeNewChart(LocationDataset::instance().data);
// }


