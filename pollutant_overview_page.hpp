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

private slots:
  void onPointHovered(const QPointF &point, bool state);

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
      {"Chloroform", 0.5},   {"112TCEthan", 0.5},   {"Atrazine", 0.5},
      {"Pb Filtered", 0.5},  {"Hg Filtered", 0.5},  {"Pb BLM Bio", 0.5},
      {"Cd Filtered", 0.5},  {"Cadmium - Cd", 0.5}, {"Chromium -Cr", 0.5},
      {"Cr- Filtered", 0.5}, {"Nitrate-N", 0.5},    {"Orthophospht", 0.5}};

  QList<PollutantCard *> pollutantCards;
  QMap<QString, QLineSeries *> seriesMap;
};
