#include "dashboard_item.hpp"
#include <QtWidgets>

DashboardItem::DashboardItem(const QString &pageTitle,
                             const QString &pageDescription, int pageIndex)
    : QGroupBox(pageTitle), pageIndex(pageIndex) {
  createMainWidget(pageDescription);
}

void DashboardItem::createMainWidget(const QString &pageDescription) {
  layout = new QVBoxLayout();

  descriptionLabel = new QLabel(pageDescription);
  descriptionLabel->setWordWrap(true);
  descriptionLabel->setMinimumHeight(40);
  layout->addWidget(descriptionLabel);

  button = new QPushButton(tr("Go to page"));
  button->setFixedWidth(100);
  layout->addWidget(button);

  this->setLayout(layout);

  connect(button, &QPushButton::clicked,
          [this]() { emit navigateToPage(pageIndex); });
}
