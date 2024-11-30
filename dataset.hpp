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

  void loadData(const std::string &filename);

  int size() const { return data.size(); }
  Sample operator[](int index) const { return data.at(index); }
  void clearData();
  bool isEmpty() const { return data.empty(); }

signals:
  void dataUpdated();

private:
  std::vector<Sample> data;

  Dataset() {}
  Dataset(const Dataset &) = delete;
  Dataset &operator=(const Dataset &) = delete;

  void checkDataExists() const;
};
