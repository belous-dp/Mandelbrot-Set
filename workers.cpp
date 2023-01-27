//
// Created by Danila Belous on 27.01.2023 at 06:36.
//

#include "workers.h"
#include <cassert>

double color_it(int policy, int it, int max) {
  // todo policy enum
  if (policy == 1) {
    return 1 - it / static_cast<double>(max);
  } else if (policy == 2) {
    // for points that are not in the set iteration is < 50
    int bottom = 50;
    return 1 - (it == max ? bottom : it % bottom) / static_cast<double>(bottom);
  } else if (policy == 3) {
    // for points that are not in the set iteration is < 50
    int bottom = 50;
    return it == max ? 0 : ((it % (bottom + 1)) / static_cast<double>(bottom));
  }
}

double get_escape_rate(QPointF const& pixel, unsigned cur_img_version, unsigned num_iterations,
                       render_layout const& lay, std::atomic<unsigned>& m_max_version) {

  QPointF c = pixel_to_pos(pixel, lay);
  double cx = c.x();
  double cy = c.y();

  double x = 0, y = 0;

  int iteration = 0;
  while (iteration < num_iterations && x * x + y * y <= 4) {
    if (cur_img_version + 1 < m_max_version.load(std::memory_order_relaxed)) {
      return -1;
    }
    double tx = x * x - y * y + cx;
    y = 2 * x * y + cy;
    x = tx;
    iteration++;
  }
  return color_it(3, iteration, num_iterations);
}

void fill_image_chunk(uchar* data, qsizetype bytes_per_line, int line, int height, render_layout lay,
                      unsigned num_iter, std::atomic<unsigned>& m_cur_version, std::atomic<unsigned>& m_max_version,
                      std::atomic<uint8_t>& m_failed) {
  for (int y = 0; y < height; ++y) {
    uchar* p = data + y * bytes_per_line;
    for (int x = 0; x < lay.m_img_width; ++x) {
      double escape_rate =
          get_escape_rate(QPointF(x, y + line), m_cur_version.load(std::memory_order_relaxed), num_iter, lay, m_max_version);
      if (escape_rate < -0.5 || m_failed.load(std::memory_order_relaxed)) {
        m_failed.fetch_or(true, std::memory_order_relaxed);
        return;
      }
      *p++ = static_cast<uchar>(escape_rate * 0xff);
      *p++ = static_cast<uchar>(escape_rate * 0.3 * 0xff);
      *p++ = 0;
    }
  }
}

void workers::fill_image(QImage& image, render_layout const& lay) {
  std::vector<std::thread> hard_workers(std::min(nthreads, std::thread::hardware_concurrency()));
  std::size_t lines_per_thread = lay.m_img_height / hard_workers.size();
  uchar* data = image.bits();
  qsizetype bytes_per_line = image.bytesPerLine();
  unsigned iter = MIN_ITER;
  while (iter <= MAX_ITER) {
    for (std::size_t i = 0, lc = 0; i < hard_workers.size(); ++i, lc += lines_per_thread) {
      int height = lines_per_thread;
      if (i + 1 == hard_workers.size()) {
        height += lay.m_img_height % hard_workers.size();
      }
      hard_workers[i] = std::thread(&fill_image_chunk, data + lc * bytes_per_line, bytes_per_line, lc, height, lay,
                                    iter, std::ref(m_cur_version), std::ref(m_max_version), std::ref(m_failed));
    }
    for (auto& t : hard_workers) {
      t.join();
    }
    if (!m_failed.load(std::memory_order_relaxed)) {
      emit image_ready(image);
    }
    iter += DELTA;
  }
}

void workers::render_image(render_layout const& lay) {
  QImage img(lay.m_img_width, lay.m_img_height, QImage::Format_RGB888);
  assert(m_cur_version <= m_max_version);
  m_failed.store(false, std::memory_order_relaxed);
  // fill_image(img, lay);
  m_perf_helper.profile([&] { fill_image(img, lay); });
  m_cur_version++;
}
