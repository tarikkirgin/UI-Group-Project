#include "window.hpp"
#include "dashboard.hpp"
#include "dataset.hpp"
#include "fluorinated_compounds_page.hpp"
#include "location_dataset.hpp"
#include "persistent_organic_pollutants_page.hpp"
#include "pollutant_overview_page.hpp"
#include "compliance_page.hpp"
#include <QtWidgets>

static const int MIN_WIDTH = 300;

Window::Window() : QMainWindow() {
  createMainWidget();
  toolBar = new QToolBar();
  addToolBar(toolBar);
  createToolBar();
  createStatusBar();
  addFileMenu();
  addHelpMenu();

  setMinimumWidth(MIN_WIDTH);
  setWindowTitle(tr("Water Quality Monitor"));
}
// defining functions in the classes
void Window::createMainWidget() {
  QMap<QString, QString> pageDetails = {
      {tr("Pollutant Overview"),
       tr("Common pollutants like 1,1,2-Trichloroethane and Chloroform.")},
      {tr("Persistent Organic Pollutants (POPs)"),
       tr("PCBs and other persistent organic pollutants with "
          "long-lasting impact "
          "on the environment and health.")},
      {tr("Environmental Litter Indicators"),
       tr("Physical pollutants, such as plastic litter and other "
          "visible debris "
          "in water.")},
      {tr("Fluorinated Compounds"),
       tr("Levels of PFAS and other fluorinated compounds monitored for their "
          "environmental persistence.")},
      {tr("Compliance Dashboard"),
       tr("Overview of regulatory compliance across all pollutants "
          "showing which "
          "substances meet or exceed safety standards.")}};

  stackedWidget = new QStackedWidget();
  setCentralWidget(stackedWidget);

  dashboard = new Dashboard(pageDetails);
  pollutant_overview_page = new PollutantOverviewPage();
  persistent_organic_pollutants_page = new PersistentOrganicPollutantsPage();
  environmental_litter_page = new EnvironmentalLitterPage();
  fluorinated_compounds_page = new FluorinatedCompoundsPage();
  compliance_page = new compliancePage();

  stackedWidget->addWidget(dashboard);
  stackedWidget->addWidget(pollutant_overview_page);
  stackedWidget->addWidget(persistent_organic_pollutants_page);
  stackedWidget->addWidget(environmental_litter_page);
  stackedWidget->addWidget(fluorinated_compounds_page);
  stackedWidget->addWidget(compliance_page);

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
  homeButton->setToolTip(tr("Home"));
  homeButton->setText(tr("Home"));
  homeButton->setAutoRaise(true);
  toolBar->addWidget(homeButton);
  toolBar->setContentsMargins(0, 0, 0, 0);

  toolBar->addSeparator();

  locationComboBox = new QComboBox();
  locationComboBox->setEditable(true);
  locationComboBox->setMinimumWidth(200);
  locationComboBox->lineEdit()->setPlaceholderText(tr("Pick location"));
  toolBar->addWidget(locationComboBox);

  toolBar->addSeparator();

  connect(locationComboBox, &QComboBox::currentTextChanged,
          &LocationDataset::instance(), &LocationDataset::onLocationChanged);

  connect(homeButton, &QToolButton::clicked, this, &Window::switchToDashboard);

  QStringList desiredOrder = {
      tr("Pollutant Overview"),
      tr("Persistent Organic Pollutants (POPs)"),
      tr("Environmental Litter Indicators"),
      tr("Fluorinated Compounds"),
      tr("Compliance Dashboard"),
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
  fileInfo = new QLabel(tr("Current file: <none>"));
  QStatusBar *status = statusBar();
  status->addWidget(fileInfo);

  QString currentLocale = QLocale::system().name();
  QLabel *localeInfo = new QLabel(tr("Locale: %1").arg(currentLocale));
  status->addPermanentWidget(localeInfo);
}

void Window::addFileMenu() {
  QAction *locAction = new QAction(tr("Open CSV File"), this);
  locAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
  connect(locAction, SIGNAL(triggered()), this, SLOT(setDataLocation()));

  QAction *closeAction = new QAction(tr("Quit"), this);
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

void Window::setDataLocation() { // 
  QString filename = QFileDialog::getOpenFileName(
      this, "Open CSV File", ".", "CSV Files (*.csv);;All Files (*)");
//"/home/rubyddong/UI/'Y-2024 (1).csv'";
  if (filename.isEmpty()) {
    return;
  }

  try {
    Dataset::instance().loadData(filename.toStdString());
  } catch (const std::exception &error) {
    QMessageBox::critical(this, tr("CSV File Error"), error.what());
    return;
  }

  QFileInfo fileInfo(filename);
  this->fileInfo->setText(
      QString(tr("Current file: <kbd>%1</kbd>")).arg(fileInfo.fileName()));
}

void Window::about() {
  QMessageBox::about(this, tr("About Water Quality Monitor"),
                     tr("Water Quality Monitor displays and analyses water "
                        "quality data loaded from a CSV file.\n\n"));
}
