#pragma once
#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QBarSet>
#include <QBarSeries>
#include "sample.hpp"
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

class compliancePage : public QWidget {
  Q_OBJECT

public:
  compliancePage();
  std::vector<std::string> existingPollutants;
  QBarCategoryAxis *axisX;
  QValueAxis *axisY;
private:
  QBarSeries *series;
  QChart *chart;
  QComboBox *pollutantComboBox;
  QComboBox *complianceComboBox;

  // QBarSet *barSet;

  void setupUI(); //Have etc and dropdowns and charts
  void updateChartDropdown();// connect to how the chart reacts to these dropdowns
  // void PupdateChart();
  // void makeNewChart(const std::vector<Sample> &dataset); //drop downs for pollutant and compliance and also location on the toolbar
  void updateDropdown();
  // void hover(); //reacting to chart when hovering
  bool checkOverlaps(std::string pollutantName);

  QChartView *chartView;
};
