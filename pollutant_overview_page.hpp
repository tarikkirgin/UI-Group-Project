#pragma once
#include <QtWidgets>
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
};
