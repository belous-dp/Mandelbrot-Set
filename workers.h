//
// Created by Danila Belous on 27.01.2023 at 06:36.
//

#pragma once
#include "perf_helper.h"
#include "render_layout.h"
#include <QImage>
#include <QObject>
#include <atomic>

class workers : public QObject {
  Q_OBJECT

public slots:
  void render_image(render_layout const& lay);

signals:
  void image_ready(QImage const& image);

public:
  std::atomic<unsigned int> m_max_version{0};

private:
  constexpr static unsigned MAX_ITER = 2000;
  constexpr static unsigned MIN_ITER = 15;
  constexpr static unsigned NSTEPS = 10;
  constexpr static unsigned DELTA = 200;

  perf_helper m_perf_helper;
  std::atomic<unsigned int> m_cur_version{0};

  void fill_image(QImage& image, render_layout const& lay);
  double get_escape_rate(int pos_x, int pos_y, unsigned cur_img_version, unsigned num_iterations,
                         render_layout const& lay);
};