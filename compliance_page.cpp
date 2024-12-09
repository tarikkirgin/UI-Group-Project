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
    setupUI();
    connect(pollutantComboBox, &QComboBox::currentTextChanged, //takes the string value of text that's changed
    &PollutantDataset::instance(), &PollutantDataset::onPollutantChanged); //send the string value to onPollutantChanged();
    connect(complianceComboBox, &QComboBox::currentTextChanged,
    &PollutantDataset::instance(), &PollutantDataset::onComplianceChanged);
    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this, &compliancePage::LupdateChart);
    connect(&PollutantDataset::instance(), &PollutantDataset::dataUpdated, this, &compliancePage::PupdateChart);
}

void compliancePage::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  QHBoxLayout *dropdown = new QHBoxLayout(this);
  //for charts!==============================
  series = new QBarSeries();
  chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Measurement/Compliance based on each pollutant");
  chart->createDefaultAxes();
  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  
  
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

  updateToolBar();

}

void compliancePage::updateToolBar() { 
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

void compliancePage::PupdateChart() {
 
  // const auto &PollutantDataset = PollutantDataset::instance().data;
  makeNewChart(PollutantDataset::instance().data);
}

void compliancePage::LupdateChart() {
 
  // const auto &LocationDataset = LocationDataset::instance().data;
  makeNewChart(LocationDataset::instance().data);
}


void compliancePage::makeNewChart(const std::vector<Sample> &dataset) {

  QBarSet *barSet = new QBarSet("Pollutants");
    for (const auto &sampleData : dataset) {
        barSet->append(std::stod(sampleData.getDeterminand().getUnit())); // change this to Qstring

        // Set color based on the boolean flag
        if (sampleData.getIsComplianceSample()) {
            barSet->setColor(Qt::green); // Green for true
        } else {
            barSet->setColor(Qt::red);   // Red for false
        }
    }    
    chart->removeAllSeries();

    // QBarSeries *series = new QBarSeries();

    series->append(barSet); // bars to series

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for (const auto &sampleData : dataset) {
      axisX->append(QString::fromStdString(sampleData.getDeterminand().getLabel()));
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX); // add to series

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Values");
    chart->addAxis(axisY, Qt::AlignLeft);  
    series->attachAxis(axisY); //add to series

    chart->addSeries(series); // all the series to chart

 }


