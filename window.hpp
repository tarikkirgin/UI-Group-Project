#pragma once

#include "dashboard.hpp"
#include "pollutant_overview_page.hpp"
#include <QMainWindow>
#include <QStackedWidget>

class Window : public QMainWindow {
  Q_OBJECT

public:
  Window();

private:
  void createMainWidget();
  void createToolBar();
  void createStatusBar();
  void addFileMenu();
  void addHelpMenu();

  QLabel *fileInfo;

  QStackedWidget *stackedWidget;
  Dashboard *dashboard;
  PollutantOverviewPage *pollutant_overview_page;

private slots:
  void switchPage(int pageIndex);
  void switchToDashboard();
  void setDataLocation();
  void about();
};
