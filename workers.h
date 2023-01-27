//
// Created by Danila Belous on 27.01.2023 at 06:36.
//

#pragma once
#include "perf_helper.h"
#include "render_layout.h"
#include <QImage>
#include <QObject>
#include <atomic>

enum class styling { classic, lecture };

class workers : public QObject {
  Q_OBJECT

public:
  workers();

public slots:
  void render_image(render_layout const& lay);

signals:
  void image_ready(QImage const& image);

public:
  std::atomic<unsigned int> m_max_version{0};

  void set_nthreads(unsigned nthreads);
  void set_styling(styling style);

private:
  constexpr static unsigned MAX_ITER = 2000;
  constexpr static unsigned MIN_ITER = 15;
  constexpr static unsigned NSTEPS = 10;
  constexpr static unsigned DELTA = 200;

  unsigned m_nthreads;
  styling m_style{styling::lecture};

  perf_helper m_perf_helper;
  std::atomic<unsigned int> m_cur_version{0};
  std::atomic<uint8_t> m_failed{false};

  void fill_image(QImage& image, render_layout const& lay);
};
