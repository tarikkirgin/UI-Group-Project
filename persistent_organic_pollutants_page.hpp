#pragma once
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class PersistentOrganicPollutantsPage : public QWidget {
  Q_OBJECT

public:
  PersistentOrganicPollutantsPage();

private:
  void setupUI();

  void updateChart();

  QChartView *chartView;
  QLineSeries *series;
};
