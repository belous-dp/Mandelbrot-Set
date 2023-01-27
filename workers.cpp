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

double workers::get_escape_rate(int pos_x, int pos_y, unsigned cur_img_version, unsigned num_iterations,
                                render_layout const& lay) {

  double cx = lay.m_min_x + ((lay.m_max_x - lay.m_min_x) * pos_x) / lay.m_img_width;
  double cy = lay.m_min_y + ((lay.m_max_y - lay.m_min_y) * pos_y) / lay.m_img_height;

  // std::cout << std::fixed << std::setprecision(3) << cx << ' ' << cy << '\n';

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
  // std::cout << iteration << ' ';
  return color_it(3, iteration, num_iterations);
}

void workers::fill_image(QImage& image, render_layout const& lay) {
  // std::cout << "drawing picture: " << image.width() << 'x' << image.height() << '\n';
  unsigned iter = MIN_ITER;
  while (iter <= MAX_ITER) {
    uchar* data = image.bits();
    for (int y = 0; y < image.height(); ++y) {
      uchar* p = data + y * image.bytesPerLine();
      for (int x = 0; x < image.width(); ++x) {

        double escape_rate = get_escape_rate(x, y, m_cur_version.load(std::memory_order_relaxed), iter, lay);
        if (escape_rate < -0.5) {
          return;
        }

        *p++ = static_cast<uchar>(escape_rate * 0xff);
        *p++ = static_cast<uchar>(escape_rate * 0.3 * 0xff);
        *p++ = 0;
      }
    }
    iter += DELTA;
    emit image_ready(image);
  }
}

void workers::render_image(render_layout const& lay) {
  QImage img(lay.m_img_width, lay.m_img_height, QImage::Format_RGB888);
  assert(m_cur_version <= m_max_version);
  fill_image(img, lay);
  // m_perf_helper.profile([&] { fill_image(img); });
  m_cur_version++;
}
