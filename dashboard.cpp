#include "dashboard.hpp"
#include "dashboard_item.hpp"

Dashboard::Dashboard() { createMainWidget(); }

void Dashboard::createMainWidget() {
  layout = new QVBoxLayout(this);

  QStringList itemLabels = {"Pollutant Overview",
                            "Persistent Organic Pollutants (POPs)",
                            "Environmental Litter Indicators",
                            "Fluorinated Compounds", "Compliance Dashboard"};

  for (int i = 0; i < itemLabels.size(); ++i) {
    DashboardItem *item = new DashboardItem(itemLabels[i], i + 1);
     
    connect(item, &DashboardItem::navigateToPage, this,
            &Dashboard::navigateToPage);      
    layout->addWidget(item);
  }
  setLayout(layout);
}
