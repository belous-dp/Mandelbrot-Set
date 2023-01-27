//
// Created by Danila Belous on 27.01.2023 at 06:36.
//

#pragma once
#include "perf_helper.h"
#include <QImage>
#include <QObject>

class workers : public QObject {
  Q_OBJECT

public slots:
  void render_image(int width, int height);

signals:
  void image_ready(QImage const& image);

private:
  perf_helper m_perf_helper;
};
