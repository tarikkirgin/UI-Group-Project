#pragma once
#include <QStringList>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class PollutantOverviewPage : public QWidget {
  Q_OBJECT

public:
  PollutantOverviewPage();

private:
  QMap<QString, double> determinands = {
      {"Chloroform", 0.75}, {"112TCEthan", 0.60}, {"Atrazine", 0.85}};
  void setupUI();
  void updateChart();

  QChartView *chartView;
  QLineSeries *series;
};
