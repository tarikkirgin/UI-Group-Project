#pragma once

#include "dashboard_item.hpp"
#include "model.hpp"
#include <QMainWindow>

class QLabel;
class QVBoxLayout;
class QPushButton;

class Dashboard : public QMainWindow {
  Q_OBJECT

public:
  Dashboard();

private:
  void createMainWidget();
  void createToolBar();
  void createStatusBar();
  void addFileMenu();
  void addHelpMenu();

  DataModel model; // data model used by table
  QVBoxLayout *layout;
  QLabel *fileInfo; // status bar info on current file
  DashboardItem *item1;
  DashboardItem *item2;
  DashboardItem *item3;
  DashboardItem *item4;
  DashboardItem *item5;

private slots:
  void setDataLocation();
  void about();
};
