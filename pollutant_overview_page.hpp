#pragma once
#include "flowlayout.h"
#include "pollutant_card.hpp"
#include <QLineEdit>
#include <QStringList>
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
  void filterCards();

  QChartView *chartView;
  QLineSeries *series;

  QLineEdit *searchBar;
  QMap<QString, double> determinandsMap = {
      {"Chloroform", 0.75}, {"112TCEthan", 0.60}, {"Atrazine", 0.85}};

  QList<PollutantCard *> pollutantCards;
};
