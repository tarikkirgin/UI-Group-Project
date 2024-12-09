#pragma once
#include "flowlayout.h"
#include "pollutant_card.hpp"
#include <QLineEdit>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

class PollutantOverviewPage : public QWidget {
  Q_OBJECT

public:
  PollutantOverviewPage();

private:
  void setupUI();
  void updateChart();
  void filterCards();

  QVBoxLayout *mainLayout;
  QVBoxLayout *contentLayout;
  QWidget *centralWidget;
  QScrollArea *scrollArea;
  QScrollArea *chartScrollArea;
  QWidget *cardContainer;
  FlowLayout *flowLayout;
  QChart *chart;
  QChartView *chartView;
  QDateTimeAxis *axisX;
  QValueAxis *axisY;

  QLineEdit *searchBar;
  QMap<QString, double> determinandsMap = {
      {"Chloroform", 0.75}, {"112TCEthan", 0.60}, {"Atrazine", 0.85}};

  QList<PollutantCard *> pollutantCards;
};
