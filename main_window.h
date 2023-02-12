//
// Created by Danila Belous on 26.01.2023 at 11:37.
//

#pragma once

#include "render_layout.h"
#include <QMainWindow>

class QLabel;
class QWidget;
class QMenu;
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
  QMenu* m_style_menu;
  QAction* m_style1_act;
  QAction* m_style2_act;
  QAction* m_style3_act;
  QLabel* m_position_label;
  QLabel* m_img_info_label;
};
