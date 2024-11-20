#pragma once

#include "sample.hpp"
#include <vector>

class Dataset {
public:
  Dataset() {}
  Dataset(const std::string &filename) { loadData(filename); }
  void loadData(const std::string &);
  int size() const { return data.size(); }
  Sample operator[](int index) const { return data.at(index); }

private:
  std::vector<Sample> data;
  void checkDataExists() const;
};
