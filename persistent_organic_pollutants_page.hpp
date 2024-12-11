#pragma once
#include "flowlayout.h"
#include "pollutant_card.hpp"
#include <QComboBox>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

constexpr double COMPLIANCE_LEVEL = 0.5;

class PersistentOrganicPollutantsPage : public QWidget {
  Q_OBJECT

public:
  PersistentOrganicPollutantsPage();

private:
  QMap<QString, double> determinandsMap = {
      {"Endosulfan", COMPLIANCE_LEVEL},
      {"PFOS (B)", COMPLIANCE_LEVEL},
      {"PFOS (L)", COMPLIANCE_LEVEL},
      {"Chlrdn-cs/Z/", COMPLIANCE_LEVEL},
      {"Chlordane-tr", COMPLIANCE_LEVEL},
      {"Dieldrin", COMPLIANCE_LEVEL},
      {"Endrin", COMPLIANCE_LEVEL},
      {"Heptachlor", COMPLIANCE_LEVEL},
      {"Hexachlorbnz", COMPLIANCE_LEVEL},
      {"Aldrin", COMPLIANCE_LEVEL},
      {"PClBenzene", COMPLIANCE_LEVEL},
      {"Mirex", COMPLIANCE_LEVEL},
      {"Toxaphene", COMPLIANCE_LEVEL},
      {"HBCDD", COMPLIANCE_LEVEL},
      {"HEXACHLORO 1", COMPLIANCE_LEVEL},
      {"Pentachlorophenol", COMPLIANCE_LEVEL},
      {"HCH Alpha", COMPLIANCE_LEVEL},
      {"HCH Gamma", COMPLIANCE_LEVEL},
      {"HCH Epsilon", COMPLIANCE_LEVEL},
      {"HCH Beta", COMPLIANCE_LEVEL},
      {"PCB Con 028", COMPLIANCE_LEVEL},
      {"PCB Con 052", COMPLIANCE_LEVEL},
      {"PCB Con 101", COMPLIANCE_LEVEL},
      {"PCB Con 105", COMPLIANCE_LEVEL},
      {"PCB Con 118", COMPLIANCE_LEVEL},
      {"PCB Con 138", COMPLIANCE_LEVEL},
      {"PCB Con 153", COMPLIANCE_LEVEL},
      {"PCB Con 156", COMPLIANCE_LEVEL},
      {"PCB Con 180", COMPLIANCE_LEVEL},
      {"PBDE 100", COMPLIANCE_LEVEL},
      {"PBDE 138", COMPLIANCE_LEVEL},
      {"PBDE 153", COMPLIANCE_LEVEL},
      {"PBDE 154", COMPLIANCE_LEVEL},
      {"PBDE 183", COMPLIANCE_LEVEL},
      {"PBDE 28", COMPLIANCE_LEVEL},
      {"PBDE 47", COMPLIANCE_LEVEL},
      {"PBDE 66", COMPLIANCE_LEVEL},
      {"PBDE 85", COMPLIANCE_LEVEL},
      {"PBDE 99", COMPLIANCE_LEVEL},
  };

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
  QBarCategoryAxis *axisX;
  QValueAxis *axisY;
  QStringList categories;
  QComboBox *filterDropdown;

  QList<PollutantCard *> pollutantCards;
  QMap<QString, QBarSet *> barSetMap;
};
