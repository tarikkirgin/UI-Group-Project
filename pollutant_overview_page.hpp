#pragma once

#include "page.hpp"

class PollutantOverviewPage : public Page {
  Q_OBJECT

public:
  PollutantOverviewPage();

private:
  void setupUI();
};
