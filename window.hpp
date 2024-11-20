#pragma once

#include "model.hpp"
#include <QMainWindow>

class QString;
class QComboBox;
class QLabel;
class QPushButton;
class QTableView;

class TestWindow : public QMainWindow {
  Q_OBJECT

public:
  TestWindow();

private:
  void createMainWidget();
  void createToolBar();
  void createStatusBar();
  void addFileMenu();
  void addHelpMenu();

  DataModel model;   // data model used by table
  QTableView *table; // table of data
  QLabel *fileInfo;  // status bar info on current file

private slots:
  void setDataLocation();
  void about();
};
