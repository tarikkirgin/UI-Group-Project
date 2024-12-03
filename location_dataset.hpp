#include "dataset.hpp"
#include "sample.hpp"
#include <vector>

class LocationDataset : public Dataset {
  Q_OBJECT

public:
  static LocationDataset &instance() {
    static LocationDataset instance;
    return instance;
  }

  std::string getLocation() const { return location; }
  std::vector<Sample> data;

public slots:
  void onLocationChanged(const QString &newLocation);

private:
  std::string location;

  void filterDataByLocation(const std::string &location);
};
