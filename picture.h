//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

#pragma once

#include <QWidget>
#include "perf_helper.h"

class picture : public QWidget {
  Q_OBJECT

public:
  explicit picture(QWidget* parent = nullptr);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  void fill_image(QImage& img);
  double get_escape_rate(int pos_x, int pos_y, int width, int height);
  perf_helper m_perf_helper;
};
