#include "dashboard.hpp"
#include "dashboard_item.hpp"
#include <QDebug>

Dashboard::Dashboard() { createMainWidget(); }

void Dashboard::createMainWidget() {
  layout = new QVBoxLayout(this);

  QStringList itemLabels = {"Pollutant Overview",
                            "Persistent Organic Pollutants (POPs)",
                            "Environmental Litter Indicators",
                            "Fluorinated Compounds", "Compliance Dashboard"};

  for (int i = 1; i < itemLabels.size() + 1; ++i) {
    DashboardItem *item = new DashboardItem(itemLabels[i-1], i);
     
    connect(item, &DashboardItem::navigateToPage, this,
            &Dashboard::navigateToPage);      
    layout->addWidget(item);
  }
  setLayout(layout);
}
