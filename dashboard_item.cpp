#include "dashboard_item.hpp"
#include <QtWidgets>
#include <iostream>
#include <stdexcept>

DashboardItem::DashboardItem(const QString &labelText) : QGroupBox() {
  createMainWidget(labelText);
}

void DashboardItem::createMainWidget(const QString &labelText) {
  layout = new QVBoxLayout();

  name = new QLabel(labelText);
  layout->addWidget(name);

  button = new QPushButton("Button");
  layout->addWidget(button);

  this->setLayout(layout);
}
