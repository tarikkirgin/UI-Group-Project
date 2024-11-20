#include "model.hpp"
#include "sample.hpp"

void DataModel::updateFromFile(const QString &filename) {
  beginResetModel();
  dataset.loadData(filename.toStdString());
  endResetModel();
}

QVariant DataModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  } else if (role == Qt::DisplayRole) {
    Sample s = dataset[index.row()];
    switch (index.column()) {
    case 0:
      return QVariant(s.getTime().c_str());
    case 1:
      return QVariant(s.getSamplingPoint().getLabel().c_str());
    case 2:
      return QVariant(s.getDeterminand().getLabel().c_str());
    case 3:
      return QVariant(s.getResult().getValue());
    }
  }

  return QVariant();
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (orientation == Qt::Vertical) {
    return QVariant(section + 1);
  }

  switch (section) {
  case 0:
    return QString("Time");
  case 1:
    return QString("Location");
  case 2:
    return QString("Determinand");
  case 3:
    return QString("Result");
  default:
    return QVariant();
  }
}
