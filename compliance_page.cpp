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
    qDebug() << "A";
    setupUI();
    qDebug() << "B";
    connect(pollutantComboBox, &QComboBox::currentTextChanged, //takes the string value of text that's changed
    &PollutantDataset::instance(), &PollutantDataset::onPollutantChanged);
    qDebug() << "C"; //send the string value to onPollutantChanged();
    connect(complianceComboBox, &QComboBox::currentTextChanged,
    &PollutantDataset::instance(), &PollutantDataset::onComplianceChanged);
    qDebug() << "D";
    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this, &compliancePage::updateDropdown);
    qDebug() << "E"; 
    connect(&PollutantDataset::instance(), &PollutantDataset::dataUpdated, this, &compliancePage::updateChart);
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
  barSet = new QBarSet("Pollutants");

  
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


void compliancePage::updateChart() {
   qDebug() << "Update Pchart triggered";

    // for (int i = 0; i < barSet->count(); ++i) {
    // barSet->replace(i, 0); // Replace each value with 0
    // }
    if (series != nullptr) {
    chart->removeSeries(series);
    delete series;
    }
    // chart->removeSeries(series); 
    // delete series;
    qDebug() << "0.5";
    series = new QBarSeries();
    chart->addSeries(series);
    qDebug() << "1";

    int i = 0;
    for (const auto &sampleData : PollutantDataset::instance().data) {
        barSet->append(sampleData.getResult().getValue()); // change this to Qstring
        qDebug() << i; i++;
        // Set color based on the boolean flag
        if (sampleData.getIsComplianceSample()) {
            barSet->setColor(Qt::green); // Green for true
        } else {
            barSet->setColor(Qt::red);   // Red for false
        }
    }    

    qDebug() << "2";
    // series = new QBarSeries();
    // chart->addSeries(series);

    // QBarSeries *series = new QBarSeries();

    series->append(barSet); // bars to series

    qDebug() << "3";

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for (const auto &sampleData : PollutantDataset::instance().data) {
      axisX->append(QString::fromStdString(sampleData.getDeterminand().getLabel()));
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX); // add to series

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Values");
    chart->addAxis(axisY, Qt::AlignLeft);  
    series->attachAxis(axisY); //add to series

 // all the series to chart

}


// void compliancePage::LupdateChart() {
//      qDebug() << "Update Lchart triggered";

//   // const auto &LocationDataset = LocationDataset::instance().data;
//   makeNewChart(LocationDataset::instance().data);
// }


