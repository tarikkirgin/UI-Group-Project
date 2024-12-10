#pragma once

#include "dashboard.hpp"
#include "fluorinated_compounds_page.hpp"
#include "persistent_organic_pollutants_page.hpp"
#include "pollutant_overview_page.hpp"
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QStackedWidget>
#include <QString>
#include <QToolBar>
#include <QToolButton>

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
  QToolBar *toolBar;
  QToolButton *homeButton;
  QComboBox *locationComboBox;

  QStackedWidget *stackedWidget;
  Dashboard *dashboard;
  PollutantOverviewPage *pollutant_overview_page;
  PersistentOrganicPollutantsPage *persistent_organic_pollutants_page;
  FluorinatedCompoundsPage *fluorinated_compounds_page;

private slots:
  void switchPage(int pageIndex);
  void updateToolBarLocations();
  void switchToDashboard();
  void setDataLocation();
  void about();
};
