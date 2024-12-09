#pragma once
#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QBarSet>
#include <QBarSeries>
#include "sample.hpp"

class compliancePage : public QWidget {
  Q_OBJECT

public:
  compliancePage();
  
private:
  QBarSeries *series;
  QChart *chart;
  QComboBox *pollutantComboBox;
  QComboBox *complianceComboBox;
  QBarSet *barSet;
  void setupUI(); //Have etc and dropdowns and charts
  void updateChart();// connect to how the chart reacts to these dropdowns
  // void PupdateChart();
  // void makeNewChart(const std::vector<Sample> &dataset); //drop downs for pollutant and compliance and also location on the toolbar
  void updateDropdown();
  // void hover(); //reacting to chart when hovering

  QChartView *chartView;
};
