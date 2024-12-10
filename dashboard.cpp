#include "dashboard.hpp"
#include "dashboard_item.hpp"
#include <QtWidgets>

Dashboard::Dashboard(const QMap<QString, QString> &pageDetails)
    : QWidget(), pageDetails(pageDetails) {
  createMainWidget();
}

void Dashboard::createMainWidget() {
  layout = new QVBoxLayout();
  layout->setSpacing(30);

  title = new QLabel();
  title->setText(tr("Water Quality Monitor"));
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet("padding: 0px; margin: 0px; font-size: 16px; "
                       "font-weight: bold;");
  layout->addWidget(title);

  QScrollArea *scrollArea = new QScrollArea();
  scrollArea->setFrameStyle(QFrame::NoFrame);
  scrollArea->setWidgetResizable(true);

  QWidget *scrollWidget = new QWidget();
  QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
  scrollLayout->setSpacing(40);

  QStringList desiredOrder = {
      tr("Pollutant Overview"),
      tr("Persistent Organic Pollutants (POPs)"),
      tr("Environmental Litter Indicators"),
      tr("Fluorinated Compounds"),
      tr("Compliance Dashboard"),
  };

  for (int i = 0; i < desiredOrder.size(); ++i) {
    DashboardItem *item =
        new DashboardItem(desiredOrder[i], pageDetails[desiredOrder[i]], i + 1);
    connect(item, &DashboardItem::navigateToPage, this,
            &Dashboard::navigateToPage);
    scrollLayout->addWidget(item);
  }

  scrollWidget->setLayout(scrollLayout);
  scrollArea->setWidget(scrollWidget);

  layout->addWidget(scrollArea);

  footerLabel = new QLabel();
  footerLabel->setText(
      tr("<a href='https://environment.data.gov.uk/water-quality/batch/"
         "measurement?area=3-34&year=2024'>Dataset Source</a> | "
         "<a href='https://environment.data.gov.uk/water-quality/view/doc/"
         "reference'>Water Quality Archive Documentation</a>"));
  footerLabel->setAlignment(Qt::AlignCenter);
  footerLabel->setOpenExternalLinks(true);
  layout->addWidget(footerLabel);

  setLayout(layout);
}
