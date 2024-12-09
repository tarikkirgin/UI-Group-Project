#pragma once

#include "sample.hpp"
#include <QObject>
#include <vector>

class Dataset : public QObject {
  Q_OBJECT

public:
  static Dataset &instance() { // this make sure only one object space is used throughout the whole project.
    static Dataset instance;
    return instance;
  }
  Dataset() {}

  void loadData(const std::string &filename);

  std::vector<Sample> data; //data is array of sample
  std::vector<std::string> getLocations();
  std::vector<std::string> getPollutants();
  std::vector<bool> getCompliances();


signals:
  void dataUpdated() const;
};
