#include "render_layout.h"
//
// Created by Danila Belous on 27.01.2023 at 21:48.
//

QPointF pixel_to_pos(QPointF const& pixel, render_layout const& lay) {
  QPointF res;
  res.setX(lay.m_min_x + ((lay.m_max_x - lay.m_min_x) * pixel.x()) / lay.m_img_width);
  res.setY(lay.m_min_y + ((lay.m_max_y - lay.m_min_y) * pixel.y()) / lay.m_img_height);
  return res;
}
