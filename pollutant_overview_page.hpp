#pragma once
#include "pollutant_card.hpp"
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class PollutantOverviewPage : public QWidget {
  Q_OBJECT

public:
  PollutantOverviewPage();

private:
  void setupUI();

  void updateChart();

  QChartView *chartView;
  QLineSeries *series;
  PollutantCard *pcard;
};
