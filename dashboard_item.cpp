#include "dashboard_item.hpp"
#include <QtWidgets>

DashboardItem::DashboardItem(const QString &labelText, int pageIndex)
    : QGroupBox(labelText), pageIndex(pageIndex) {
  createMainWidget(labelText);
}

void DashboardItem::createMainWidget(const QString &labelText) {
  layout = new QVBoxLayout(this);

  button = new QPushButton("Go to page");
  layout->addWidget(button);

  this->setLayout(layout);

  connect(button, &QPushButton::clicked,
          [this]() { emit navigateToPage(pageIndex); });
}
