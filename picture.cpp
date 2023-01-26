#include "picture.h"
#include <QPainter>
#include <iostream>
//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

picture::picture(QWidget* parent) : QWidget(parent) {
  // move(0, 0);

  // QPalette pal = QPalette();
  // pal.setColor(QPalette::Window, Qt::black);
  // setAutoFillBackground(true);
  // setPalette(pal);
}

void picture::fill_image(QImage& image) {
  uchar* data = image.bits();
  for (int y = 0; y < image.height(); ++y) {
    uchar* p = data + y * image.bytesPerLine();
    for (int x = 0; x < image.width(); ++x) {

      double escape_rate = get_escape_rate(x, y, image.width(), image.height());

      // std::cout << escape_rate << '\n';

      *p++ = static_cast<uchar>(escape_rate * 0xff);
      *p++ = static_cast<uchar>(escape_rate * 0.3 * 0xff);
      *p++ = 0;
    }
  }
}

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

double picture::get_escape_rate(int pos_x, int pos_y, int width, int height) {
  double min_x = -2, max_x = 1, min_y = -1, max_y = 1;
  double cx = min_x + ((max_x - min_x) * pos_x) / width;
  double cy = min_y + ((max_y - min_y) * pos_y) / height;

  // std::cout << std::fixed << std::setprecision(3) << cx << ' ' << cy << '\n';

  double x = 0, y = 0;

  int num_iterations = 1000;
  int iteration = 0;
  while (iteration < num_iterations && x * x + y * y <= 4) {
    double tx = x * x - y * y + cx;
    y = 2 * x * y + cy;
    x = tx;
    iteration++;
  }
  //std::cout << iteration << ' ';
  return color_it(3, iteration, num_iterations);
}

void picture::paintEvent(QPaintEvent* event) {
  QPainter p(this);
  // p.drawLine(QLine(100, 200, 80, 30));
  QImage img(width(), height(), QImage::Format_RGB888);
  fill_image(img);
  p.drawImage(0, 0, img);
}
