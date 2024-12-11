#pragma once
#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QBarSet>
#include <QBarSeries>
#include "sample.hpp"
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QGridLayout>
#include <vector>

class compliancePage : public QWidget {
  Q_OBJECT

public:
  compliancePage();
  static compliancePage &instance() { // this make sure only one object space is used throughout the whole project.
    static compliancePage instance;
    return instance;
  }
  QBarCategoryAxis *axisX;
  QValueAxis *axisY;
  double average;
  double min;
  double MAX;
  QScrollArea *scrollArea;
  QVBoxLayout *mainLayout;
  // std::vector<QWidget *> cardHolder;
  // std::vector<QVBoxLayout *> cardLayout;
  // std::vector<QLabel *> valueLabel;
  // std::vector<QLabel *> complianceLabel;
  QWidget *cards;
  QGridLayout *cardsLayout = new QGridLayout();
  std::string getLevel(Sample sample); 
  QChart *chart;
  QChartView *chartView;
  QBarSeries *series;
private:
  QComboBox *pollutantComboBox;
  QComboBox *complianceComboBox;
  void setupUI(); //Have etc and dropdowns and charts
  void updateDropdownChart();
  void ungetCardUI();
  void card();
  void getAverage();
  void getminMAX();
  void getCardUI();
};
