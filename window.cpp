#include "window.hpp"
#include "dashboard.hpp"
#include "dataset.hpp"
#include "pollutant_overview_page.hpp"
#include <QtWidgets>

static const int MIN_WIDTH = 300;
Window::Window() : QMainWindow() {
  createMainWidget();
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

  dashboard = new Dashboard();
  pollutant_overview_page = new PollutantOverviewPage();

  stackedWidget->addWidget(dashboard);
  stackedWidget->addWidget(pollutant_overview_page);

  stackedWidget->setCurrentWidget(dashboard);

  connect(dashboard, &Dashboard::navigateToPage, this, &Window::switchPage);
  connect(pollutant_overview_page, &PollutantOverviewPage::navigateBack, this,
          &Window::switchToDashboard);
}

void Window::switchPage(int pageIndex) {
  stackedWidget->setCurrentIndex(pageIndex);
}

void Window::switchToDashboard() { switchPage(0); }

void Window::createToolBar() {
  QToolBar *toolBar = new QToolBar();
  addToolBar(toolBar);
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
                     "quality data loaded from"
                     "a CSV file.\n\n");
}
