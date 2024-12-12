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
#include "flowlayout.h"

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
  static double average;
  double min;
  double MAX;
  QScrollArea *scrollArea;
  QVBoxLayout *mainLayout;
  QHBoxLayout *mainWithScrollLayout;
  std::vector<QBarSet *> barSets;
  QWidget *cardContainer;
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
  void getAverage();
private slots :
  void updateDropdownChart();
  void getCardUI();
private:
  QComboBox *pollutantComboBox;
  QComboBox *complianceComboBox;
  void setupUI(); //Have etc and dropdowns and charts


  void getminMAX(std::vector<Sample> dataset);

};

class everyPollutantCard : public QGroupBox {
  Q_OBJECT

public:

  everyPollutantCard(int j, double averageTemp);
  int i;
  double average;
  // Sample *sample;
private slots:
  void updateUI();
private:
  void setupUI();
  QLabel *timeLabel;
  QLabel *valueLabel;
  QLabel *complianceLabel;
};
