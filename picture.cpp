//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

#include "picture.h"
#include <QPainter>
#include <QResizeEvent>
#include <QWheelEvent>
#include <iostream>

picture::picture(QWidget* parent) : QWidget(parent), m_image_pos({0, 0}) {

  reset_layout();

  m_workers.moveToThread(&m_workers_thread);
  connect(this, &picture::render_image, &m_workers, &workers::render_image);
  connect(&m_workers, &workers::image_ready, this, &picture::image_ready);
  m_workers_thread.start();
}

picture::~picture() {
  m_workers_thread.quit();
  m_workers_thread.wait();
}

void picture::image_ready(QImage const& image) {
  m_image = image;
  update();
}

void picture::emit_signal() {
  emit render_image(lay);
  m_workers.m_max_version++;
}

void picture::resizeEvent(QResizeEvent* event) {
  if (event->oldSize().width() != width() || event->oldSize().height() != height()) {
    reset_layout();
    emit_signal();
  }
  // std::cout << "sent render call for w=" << width() << ", h=" << height() << std::endl;
}

void picture::reset_layout() {
  lay.m_min_x = INIT_MIN_X;
  lay.m_max_x = INIT_MAX_X;
  lay.m_min_y = INIT_MIN_Y;
  lay.m_max_y = INIT_MAX_Y;
  lay.m_scale = 1.;
  lay.m_img_width = width();
  lay.m_img_height = height();
  if (width() < (lay.m_max_x - lay.m_min_x) * height() / (lay.m_max_y - lay.m_min_y)) {
    // std::cout << "resize cuz width too small\n";
    double coord_height = ((lay.m_max_x - lay.m_min_x) * height()) / width();
    lay.m_max_y = coord_height / 2;
    lay.m_min_y = -lay.m_max_y;
  } else {
    // std::cout << "resize cuz height too small\n";
    double coord_width = ((lay.m_max_y - lay.m_min_y) * width()) / height();
    lay.m_max_x = coord_width / 3;
    lay.m_min_x = -lay.m_max_x * 2;
  }
}

void picture::paintEvent(QPaintEvent* event) {
  QPainter p(this);
  // std::cout << "paint event, w=" << width() << ", h=" << height() << std::endl;
  p.fillRect(rect(), Qt::black);
  if (m_image.isNull()) {
    p.setPen(Qt::white);
    p.drawText(rect(), Qt::AlignCenter, tr("Initial rendering..."));
  } else {
    // можно было бы прямо здесь вызывать invokeMethod у workers,
    // но иногда нам не требуется заново рендерить картинку,
    // поэтому ответственность за запросы о рендеринге можно возложить
    // на те функции, в которых это действительно нужно
    p.drawImage(m_image_pos.x(), m_image_pos.y(), m_image);
    m_image_pos = {0, 0};
  }
}

void picture::wheelEvent(QWheelEvent* event) {
  int degrees = event->angleDelta().y() / 8;
  double steps = degrees / 15.;
  zoom_picture(steps);
}

void picture::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    m_press_pos = event->position();
    emit mouse_pos_changed(pixel_to_pos(m_press_pos, lay));
  }
}

void picture::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() & Qt::LeftButton) {
    QPointF image_pos = event->position() - m_press_pos;
    m_image_pos.setX(static_cast<int>(image_pos.x()));
    m_image_pos.setY(static_cast<int>(image_pos.y()));
    emit mouse_pos_changed(pixel_to_pos(m_press_pos, lay));
    update();
  }
}

void picture::mouseReleaseEvent(QMouseEvent* event) {
  QPointF delta = event->position() - m_press_pos;
  double px = delta.x() / lay.m_img_width;
  double py = delta.y() / lay.m_img_height;
  double lenx = lay.m_max_x - lay.m_min_x;
  double leny = lay.m_max_y - lay.m_min_y;
  lay.m_min_x -= lenx * px;
  lay.m_max_x -= lenx * px;
  lay.m_min_y -= leny * py;
  lay.m_max_y -= leny * py;
  emit mouse_pos_changed(pixel_to_pos(m_press_pos, lay));
  update();
  emit_signal();
}

void picture::closeEvent(QCloseEvent* event) {
  lay = {};
  emit_signal();
  event->accept();
}

void picture::zoom_picture(double power) {
  constexpr static double zoom_coef = 0.7;
  double zoom_val = pow(zoom_coef, power);
  lay.m_min_x *= zoom_val;
  lay.m_max_x *= zoom_val;
  lay.m_min_y *= zoom_val;
  lay.m_max_y *= zoom_val;
  lay.m_scale *= zoom_val;
  update();
  emit_signal();
}