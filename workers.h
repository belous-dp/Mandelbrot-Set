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

  void set_nthreads(unsigned int nthreads);

private:
  constexpr static ushort NSTEPS = 8;
  constexpr static ushort STOP = 6000;
  constexpr static ushort MIN_ITER_INIT = 100;
  constexpr static ushort MAX_ITER_INIT = 6000;
  ushort iter_start = 100;
  ushort iter_step = 100;

  unsigned int m_nthreads;

  coloring m_style{coloring::fiery};
  constexpr static ushort NCOLORS = 16;
  uint m_mapping[NCOLORS];

  //perf_helper m_perf_helper;
  std::atomic<uint32_t> m_cur_version{0};
  std::atomic<uint8_t> m_failed{false};

  void fill_image(QImage& image, render_layout const& lay);
};
