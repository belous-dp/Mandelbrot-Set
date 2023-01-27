//
// Created by Danila Belous on 27.01.2023 at 13:56.
//

#pragma once

#include <QPoint>

struct render_layout {
  double m_min_x;
  double m_max_x;
  double m_min_y;
  double m_max_y;
  double m_scale;
  int m_img_width;
  int m_img_height;
};

QPointF pixel_to_pos(QPointF const& pixel, render_layout const& lay);
