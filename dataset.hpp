#pragma once

#include "sample.hpp"
#include <QObject>
#include <vector>

class Dataset : public QObject {
  Q_OBJECT

public:
  static Dataset &instance() {
    static Dataset instance;
    return instance;
  }
  Dataset() {}

  void loadData(const std::string &filename);

  std::vector<Sample> data; //data is array of sample
  std::vector<std::string> getLocations();

signals:
  void dataUpdated() const;
};
