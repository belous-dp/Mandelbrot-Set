//
// Created by Danila Belous on 26.01.2023 at 11:37.
//

#pragma once

#include "render_layout.h"
#include <QMainWindow>

class QWidget;
class QLabel;
class picture;

class window : public QMainWindow {
  Q_OBJECT

public:
  explicit window(QWidget* parent = nullptr);

protected:
  void closeEvent(QCloseEvent* event) override;

public slots:
  void mouse_changed(QPointF const& mouse_pos);
  void window_changed(render_layout const& lay);

private:
  picture* m_picture;
  QLabel* m_position_label;
  QLabel* m_img_info_label;
};
