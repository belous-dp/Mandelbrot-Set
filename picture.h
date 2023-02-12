//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

#pragma once

#include "render_layout.h"
#include "workers.h"
#include <QThread>
#include <QWidget>

struct QSinglePointEvent;

class picture : public QWidget {
  Q_OBJECT

public:
  explicit picture(QWidget* parent = nullptr);
  ~picture();

  render_layout get_layout() const;

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

public slots:
  void style1();
  //void style2();
  //void style3();

private slots:
  void image_ready(QImage const& image);

signals:
  void render_image(render_layout const& lay, double scale_factor);
  void change_style(coloring style);
  void mouse_pos_changed(QPointF const& mouse_pos);
  void window_changed(render_layout const& lay);

private:
  QThread m_workers_thread;
  workers m_workers;
  QImage m_image;
  render_layout m_lay;

  constexpr static QPointF INIT_MIN = {-2, -1.5};
  constexpr static QPointF INIT_MAX = {1, 1.5};

  void emit_render_signal(std::string from);
  void emit_render_signal();
  void emit_stop_signal(std::string from);
  void emit_stop_signal();
  void reset_layout();
  void update_mouse(QSinglePointEvent const* event);
  void zoom_picture(double power);

  QPointF m_mouse_press_ppos;
  QPoint m_image_delta;
  double m_image_scale;
};
