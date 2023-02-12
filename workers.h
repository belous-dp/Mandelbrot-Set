//
// Created by Danila Belous on 27.01.2023 at 06:36.
//

#pragma once
#include "perf_helper.h"
#include "render_layout.h"
#include <QImage>
#include <QObject>
#include <atomic>
#include "coloring.h"

class workers : public QObject {
  Q_OBJECT

public:
  workers();

public slots:
  void render_image(render_layout const& lay, double scale_factor);
  void set_style(coloring style);

signals:
  void image_ready(QImage const& image);

public:
  std::atomic<unsigned int> m_max_version{0};

  void set_nthreads(unsigned nthreads);

private:
  constexpr static unsigned NSTEPS = 10;
  constexpr static unsigned STOP = 6000;
  unsigned iter_start = 100;
  unsigned iter_step = 100;

  unsigned m_nthreads;
  coloring m_style{coloring::lecture};

  //perf_helper m_perf_helper;
  std::atomic<unsigned int> m_cur_version{0};
  std::atomic<uint8_t> m_failed{false};

  void fill_image(QImage& image, render_layout const& lay);
};
