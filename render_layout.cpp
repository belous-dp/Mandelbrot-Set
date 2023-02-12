#include "render_layout.h"
//
// Created by Danila Belous on 27.01.2023 at 21:48.
//

QPointF pixel_to_pos(QPointF const& pixel, render_layout const& lay) {
  QPointF res;
  res.setX(lay.m_min.x() + (lay.len_x() * pixel.x()) / lay.m_img_size.width());
  res.setY(lay.m_min.y() + (lay.len_y() * pixel.y()) / lay.m_img_size.height());
  return res;
}

bool render_layout::is_null() const {
  return m_img_size.isEmpty();
}

double render_layout::len_x() const {
  return m_max.x() - m_min.x();
}

double render_layout::len_y() const {
  return m_max.y() - m_min.y();
}
