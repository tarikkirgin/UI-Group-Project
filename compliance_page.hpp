#pragma once
#include "flowlayout.h"
#include "pollutant_card.hpp"
#include <QComboBox>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QValueAxis>
#include <QtWidgets>

class compliancePage : public QWidget {
  Q_OBJECT

public:
  compliancePage();

private:
  void setupUI();
  void updateChart();
  void updateCards();
  void filterCards();

  QVBoxLayout *mainLayout;
  QVBoxLayout *contentLayout;
  QWidget *centralWidget;
  QScrollArea *scrollArea;
  QScrollArea *chartScrollArea;
  QWidget *cardContainer;
  FlowLayout *flowLayout;
  QChart *complianceChart;
  QChartView *complianceChartView;
  QPieSeries *compliancePieSeries;
  QChartView *chartView;
  QDateTimeAxis *axisX;
  QValueAxis *axisY;
  QLineEdit *searchBar;
  QComboBox *complianceLevelFilter;

  QMap<QString, PollutantCard *> pollutantCardsMap;
};
