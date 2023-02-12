//
// Created by Danila Belous on 27.01.2023 at 13:56.
//

#pragma once

#include <QPoint>
#include <QSize>

struct render_layout {
  QPointF m_min;
  QPointF m_max;
  QSize m_img_size;
  bool is_null() const;
  double len_x() const;
  double len_y() const;
};

QPointF pixel_to_pos(QPointF const& pixel, render_layout const& lay);
