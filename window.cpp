#include "window.hpp"
#include "dashboard.hpp"
#include "dataset.hpp"
#include "location_dataset.hpp"
#include "persistent_organic_pollutants_page.hpp"
#include "pollutant_overview_page.hpp"
#include <QtWidgets>

static const int MIN_WIDTH = 300;

static const QMap<QString, QString> pageDetails = {
    {"Pollutant Overview",
     "Common pollutants like 1,1,2-Trichloroethane and Chloroform."},
    {"Persistent Organic Pollutants (POPs)",
     "PCBs and other persistent organic pollutants with long-lasting impact "
     "on the environment and health."},
    {"Environmental Litter Indicators",
     "Physical pollutants, such as plastic litter and other visible debris "
     "in water."},
    {"Fluorinated Compounds",
     "Levels of PFAS and other fluorinated compounds monitored for their "
     "environmental persistence."},
    {"Compliance Dashboard",
     "Overview of regulatory compliance across all pollutants showing which "
     "substances meet or exceed safety standards."}};

Window::Window() : QMainWindow() {
  createMainWidget();
  toolBar = new QToolBar();
  addToolBar(toolBar);
  createToolBar();
  createStatusBar();
  addFileMenu();
  addHelpMenu();

  setMinimumWidth(MIN_WIDTH);
  setWindowTitle("Water Quality Monitor");
}

void Window::createMainWidget() {
  stackedWidget = new QStackedWidget();
  setCentralWidget(stackedWidget);

  dashboard = new Dashboard(pageDetails);
  pollutant_overview_page = new PollutantOverviewPage();
  persistent_organic_pollutants_page = new PersistentOrganicPollutantsPage();
  environmental_litter_page = new EnvironmentalLitterPage();

  stackedWidget->addWidget(dashboard);
  stackedWidget->addWidget(pollutant_overview_page);
  stackedWidget->addWidget(persistent_organic_pollutants_page);
  stackedWidget->addWidget(environmental_litter_page);

  stackedWidget->setCurrentWidget(dashboard);

  connect(dashboard, &Dashboard::navigateToPage, this, &Window::switchPage);
  connect(&Dataset::instance(), &Dataset::dataUpdated, this,
          &Window::updateToolBarLocations);
}

void Window::switchPage(int pageIndex) {
  stackedWidget->setCurrentIndex(pageIndex);
}

void Window::switchToDashboard() { switchPage(0); }

void Window::createToolBar() {
  homeButton = new QToolButton();
  homeButton->setToolTip("Home");
  homeButton->setText("Home");
  homeButton->setAutoRaise(true);
  toolBar->addWidget(homeButton);
  toolBar->setContentsMargins(0, 0, 0, 0);

  toolBar->addSeparator();

  locationComboBox = new QComboBox();
  locationComboBox->setEditable(true);
  locationComboBox->setMinimumWidth(200);
  locationComboBox->lineEdit()->setPlaceholderText("Pick location");
  toolBar->addWidget(locationComboBox);

  toolBar->addSeparator();

  connect(locationComboBox, &QComboBox::currentTextChanged,
          &LocationDataset::instance(), &LocationDataset::onLocationChanged);

  connect(homeButton, &QToolButton::clicked, this, &Window::switchToDashboard);

  QStringList desiredOrder = {
      "Pollutant Overview",
      "Persistent Organic Pollutants (POPs)",
      "Environmental Litter Indicators",
      "Fluorinated Compounds",
      "Compliance Dashboard",
  };

  for (int i = 0; i < desiredOrder.size(); ++i) {
    QToolButton *pageButton = new QToolButton();
    pageButton->setToolTip(desiredOrder[i]);
    pageButton->setText(desiredOrder[i]);
    pageButton->setAutoRaise(true);
    toolBar->addWidget(pageButton);
    toolBar->setContentsMargins(0, 0, 0, 0);

    connect(pageButton, &QToolButton::clicked, this,
            [this, i]() { switchPage(i + 1); });
  }
}

void Window::updateToolBarLocations() {
  std::vector<std::string> locations = Dataset::instance().getLocations();
  std::sort(locations.begin(), locations.end());

  QStringList locationList;
  for (const auto &location : locations) {
    locationList << QString::fromStdString(location);
  }
  locationComboBox->clear();
  locationComboBox->addItems(locationList);

  QCompleter *completer = new QCompleter(locationList, locationComboBox);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  locationComboBox->setCompleter(completer);
}

void Window::createStatusBar() {
  fileInfo = new QLabel("Current file: <none>");
  QStatusBar *status = statusBar();
  status->addWidget(fileInfo);
}

void Window::addFileMenu() {
  QAction *locAction = new QAction("Open CSV File", this);
  locAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
  connect(locAction, SIGNAL(triggered()), this, SLOT(setDataLocation()));

  QAction *closeAction = new QAction("Quit", this);
  closeAction->setShortcut(QKeySequence::Close);
  connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

  QMenu *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(locAction);
  fileMenu->addAction(closeAction);
}

void Window::addHelpMenu() {
  QAction *aboutAction = new QAction("&About", this);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  QAction *aboutQtAction = new QAction("About &Qt", this);
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  QMenu *helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
}

void Window::setDataLocation() {
  QString filename = QFileDialog::getOpenFileName(
      this, "Open CSV File", ".", "CSV Files (*.csv);;All Files (*)");

  if (filename.isEmpty()) {
    return;
  }

  try {
    Dataset::instance().loadData(filename.toStdString());
  } catch (const std::exception &error) {
    QMessageBox::critical(this, "CSV File Error", error.what());
    return;
  }

  QFileInfo fileInfo(filename);
  this->fileInfo->setText(
      QString("Current file: <kbd>%1</kbd>").arg(fileInfo.fileName()));
}

void Window::about() {
  QMessageBox::about(this, "About Water Quality Monitor",
                     "Water Quality Monitor displays and analyses water "
                     "quality data loaded from "
                     "a CSV file.\n\n");
}
