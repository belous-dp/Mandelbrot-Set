//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

#pragma once

#include "render_layout.h"
#include "workers.h"
#include <QThread>
#include <QWidget>

class picture : public QWidget {
  Q_OBJECT

public:
  explicit picture(QWidget* parent = nullptr);
  ~picture();

  render_layout lay;

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
  void image_ready(QImage const& image);

signals:
  void render_image(render_layout const& lay);

private:
  QThread m_workers_thread;
  workers m_workers;
  QImage m_image;

private:
  constexpr static double INIT_MIN_X = -2;
  constexpr static double INIT_MAX_X = 1;
  constexpr static double INIT_MIN_Y = -1.5;
  constexpr static double INIT_MAX_Y = 1.5;

  void emit_signal();
  void reset_layout();
  void zoom_picture(double power);

  QPointF m_press_pos;
};
