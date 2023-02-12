//
// Created by Danila Belous on 27.01.2023 at 06:36.
//

#include "workers.h"
#include <cassert>
#include <iostream>

workers::workers() : m_nthreads(std::thread::hardware_concurrency()) {}

void workers::set_nthreads(unsigned nthreads) {
  if (nthreads > 0) {
    m_nthreads = nthreads;
  }
}

int get_escape_rate(QPointF const& pixel, unsigned cur_img_version, unsigned num_iterations,
                       render_layout const& lay, std::atomic<unsigned>& m_max_version) {

  QPointF c = pixel_to_pos(pixel, lay);
  double cx = c.x();
  double cy = c.y();

  double x = 0, y = 0, x2 = 0, y2 = 0;

  int iteration = 0;
  while (iteration < num_iterations && x2 + y2 <= 4) {
    if (cur_img_version + 1 < m_max_version.load(std::memory_order_relaxed)) {
      return -1;
    }
    y = 2 * x * y + cy;
    x = x2 - y2 + cx;
    x2 = x * x;
    y2 = y * y;
    iteration++;
  }
  return iteration;
}

void color_pixel(uchar*& p, coloring style, int iter, int max) {
  if (style == coloring::pure) {
    //double val = 1 - iter / static_cast<double>(max);
    //*p++ = 0;
    //*p++ = static_cast<uchar>(val * 0xff);
    //*p++ = static_cast<uchar>(val * 0.3 * 0xff);
    if (iter == max) {
      *p++ = *p++ = *p++ = 0;
    } else {
      *p++ = *p++ = *p++ = static_cast<uchar>(0xff);
    }
  } else if (style == coloring::fiery) {
    int bottom = 50;
    double val = iter == max ? 0 : ((iter % (bottom + 1)) / static_cast<double>(bottom));
    *p++ = static_cast<uchar>(val * 0xff);
    *p++ = static_cast<uchar>(val * 0.3 * 0xff);
    *p++ = 0;
  }
}

void fill_image_chunk(uchar* data, qsizetype bytes_per_line, int line, int height, coloring style, render_layout lay,
                      unsigned num_iter, std::atomic<unsigned>& m_cur_version, std::atomic<unsigned>& m_max_version,
                      std::atomic<uint8_t>& m_failed) {
  for (int y = 0; y < height; ++y) {
    uchar* p = data + y * bytes_per_line;
    for (int x = 0; x < lay.m_img_size.width(); ++x) {
      int iterations = get_escape_rate(QPointF(x, y + line), m_cur_version.load(std::memory_order_relaxed),
                                           num_iter, lay, m_max_version);
      if (iterations < 0 || m_failed.load(std::memory_order_relaxed)) {
        m_failed.fetch_or(true, std::memory_order_relaxed);
        return;
      }
      color_pixel(p, style, iterations, num_iter);
    }
  }
}

void workers::fill_image(QImage& image, render_layout const& lay) {
  std::vector<std::thread> hard_workers(m_nthreads);
  std::size_t lines_per_thread = lay.m_img_size.height() / hard_workers.size();
  uchar* data = image.bits();
  qsizetype bytes_per_line = image.bytesPerLine();
  unsigned iter = iter_start;
  for (unsigned iter = iter_start, step = 0; step < NSTEPS && (step < 2 || iter < STOP); iter += iter_step, ++step) {
    for (std::size_t i = 0, lc = 0; i < hard_workers.size(); ++i, lc += lines_per_thread) {
      int height = lines_per_thread;
      if (i + 1 == hard_workers.size()) {
        height += lay.m_img_size.height() % hard_workers.size();
      }
      hard_workers[i] = std::thread(&fill_image_chunk, data + lc * bytes_per_line, bytes_per_line, lc, height, m_style,
                                    lay, iter, std::ref(m_cur_version), std::ref(m_max_version), std::ref(m_failed));
    }
    for (auto& t : hard_workers) {
      t.join();
    }
    if (!m_failed.load(std::memory_order_relaxed)) {
      emit image_ready(image);
    } else {
      break;
    }
  }
}

void workers::render_image(render_layout const& lay, double scale_factor) { // maybe it'd be better to pass a copy?
  if (lay.is_null()) { // stop signal
    // do nothing
    m_cur_version++;
    return;
  }
  QImage img(lay.m_img_size.width(), lay.m_img_size.height(), QImage::Format_RGB888);
  assert(m_cur_version <= m_max_version);
  m_failed.store(false, std::memory_order_relaxed);
  fill_image(img, lay);
  //m_perf_helper.profile([&] {
    //m_failed.store(false, std::memory_order_relaxed);
    //fill_image(img, m_lay);
  //});
  if (scale_factor >= 1) {
    if (iter_start < 2000) {
      scale_factor = log(scale_factor) / (1 + log(scale_factor)) / 2.5;
    } else {
      scale_factor = (atan(scale_factor) - atan(1)) / 10;
    }
    iter_start *= (1 + scale_factor);
    iter_step *= (1 + scale_factor / 2.2);
  } else {
    assert(scale_factor >= 0);
    if (iter_start < 2000) {
      scale_factor = -log(scale_factor) / (1 - log(scale_factor)) / 2.5;
    } else {
      scale_factor = (atan(1 / scale_factor) - atan(1)) / 10;
    }
    iter_start /= (1 + scale_factor);
    iter_step /= (1 + scale_factor / 2.2);
  }
  std::cout << iter_start << ' ' << iter_step << std::endl;
  m_cur_version++;
}

void workers::set_style(coloring style) {
  m_style = style;
}
