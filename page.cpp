#include "page.hpp"

Page::Page(const QString &title) : QWidget() { setupUI(title); }

void Page::setupUI(const QString &title) {
  layout = new QVBoxLayout(this);

  titleLabel = new QLabel(title, this);
  titleLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(titleLabel);

  backButton = new QPushButton("Back to Dashboard", this);
  connect(backButton, &QPushButton::clicked, this, &Page::navigateBack);
  layout->addWidget(backButton);

  setLayout(layout);
}
