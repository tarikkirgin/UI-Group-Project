#include "dashboard.hpp"
#include <QtWidgets>
#include <iostream>
#include <stdexcept>

static const int MIN_WIDTH = 620;

Dashboard::Dashboard() : QMainWindow() {
  createMainWidget();
  createToolBar();
  createStatusBar();
  addFileMenu();
  addHelpMenu();

  setMinimumWidth(MIN_WIDTH);
  setWindowTitle("Water Quality Monitor");
}

void Dashboard::createMainWidget() {
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  layout = new QVBoxLayout();

  item1 = new DashboardItem("item 1");
  item2 = new DashboardItem("item 2");
  item3 = new DashboardItem("item 3");
  item4 = new DashboardItem("item 4");
  item5 = new DashboardItem("item 5");

  layout->addWidget(item1);
  layout->addWidget(item2);
  layout->addWidget(item3);
  layout->addWidget(item4);
  layout->addWidget(item5);

  centralWidget->setLayout(layout);
}

void Dashboard::createToolBar() {
  QToolBar *toolBar = new QToolBar();
  addToolBar(toolBar);
}

void Dashboard::createStatusBar() {
  fileInfo = new QLabel("Current file: <none>");
  QStatusBar *status = statusBar();
  status->addWidget(fileInfo);
}

void Dashboard::addFileMenu() {
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

void Dashboard::addHelpMenu() {
  QAction *aboutAction = new QAction("&About", this);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  QAction *aboutQtAction = new QAction("About &Qt", this);
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  QMenu *helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
}

void Dashboard::setDataLocation() {
  QString filename = QFileDialog::getOpenFileName(
      this, "Open CSV File", ".", "CSV Files (*.csv);;All Files (*)");

  if (filename.isEmpty()) {
    return;
  }

  try {
    model.updateFromFile(filename);
  } catch (const std::exception &error) {
    QMessageBox::critical(this, "CSV File Error", error.what());
    return;
  }

  QFileInfo fileInfo(filename);
  this->fileInfo->setText(
      QString("Current file: <kbd>%1</kbd>").arg(fileInfo.fileName()));

  // table->resizeColumnsToContents();
}

void Dashboard::about() {
  QMessageBox::about(this, "About Water Quality Monitor",
                     "Water Quality Monitor displays and analyses water "
                     "quality data loaded from"
                     "a CSV file.\n\n");
}
