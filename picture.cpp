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

render_layout picture::get_layout() const {
  return m_lay;
}

void picture::image_ready(QImage const& image) {
  m_image = image;
  // std::cout << "picture::image_ready. updating (scheduling repaint)\n";
  update();
}

void picture::emit_render_signal(std::string from) {
  // std::cout << "picture::emit_render_signal from function " + from + '\n';
  emit render_image(m_lay);
  m_workers.m_max_version++;
}

void picture::emit_stop_signal(std::string from) {
  // std::cout << "picture::emit_stop_signal from function " + from + '\n';
  m_lay.m_scale = 0;
  emit render_image(m_lay);
  m_workers.m_max_version++;
}

void picture::resizeEvent(QResizeEvent* event) {
  // std::cout << "picture::resizeEvent: ";
  if (event->oldSize().width() != width() || event->oldSize().height() != height()) {
    // std::cout << "new size\n";
    reset_layout();
    emit_render_signal("resizeEvent");
  } else {
    // std::cout << "same size\n";
  }
}

void picture::reset_layout() {
  m_lay.m_min_x = INIT_MIN_X;
  m_lay.m_max_x = INIT_MAX_X;
  m_lay.m_min_y = INIT_MIN_Y;
  m_lay.m_max_y = INIT_MAX_Y;
  m_lay.m_scale = 1.;
  m_lay.m_img_width = width();
  m_lay.m_img_height = height();
  if (width() < (m_lay.m_max_x - m_lay.m_min_x) * height() / (m_lay.m_max_y - m_lay.m_min_y)) {
    double coord_height = ((m_lay.m_max_x - m_lay.m_min_x) * height()) / width();
    m_lay.m_max_y = coord_height / 2;
    m_lay.m_min_y = -m_lay.m_max_y;
  } else {
    double coord_width = ((m_lay.m_max_y - m_lay.m_min_y) * width()) / height();
    m_lay.m_max_x = coord_width / 3;
    m_lay.m_min_x = -m_lay.m_max_x * 2;
  }
}

void picture::paintEvent(QPaintEvent* event) {
  QPainter p(this);
  p.fillRect(rect(), Qt::black);
  // std::cout << "picture::paintEvent: ";
  if (m_image.isNull()) {
    // std::cout << "initial painting\n"; // todo remove cuz useless?
    p.setPen(Qt::white);
    p.drawText(rect(), Qt::AlignCenter, tr("Initial rendering..."));
  } else {
    // std::cout << "redrawing, x=" << m_image_pos.x() << ", y=" << m_image_pos.y() << '\n';
    p.drawImage(m_image_pos.x(), m_image_pos.y(), m_image);
    m_image_pos = {0, 0};
    m_lay.m_scale = 1.;
  }
}

void picture::wheelEvent(QWheelEvent* event) {
  int degrees = event->angleDelta().y() / 8;
  double steps = degrees / 15.;
  zoom_picture(steps);
}

void picture::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    // std::cout << "picture::mousePressEvent\n";
    m_press_pos = event->position();
    emit_stop_signal("mousePressEvent");
    emit mouse_pos_changed(pixel_to_pos(m_press_pos, m_lay));
  }
}

void picture::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() & Qt::LeftButton) {
    QPointF image_pos = event->position() - m_press_pos;
    m_image_pos.setX(static_cast<int>(image_pos.x()));
    m_image_pos.setY(static_cast<int>(image_pos.y()));
    emit mouse_pos_changed(pixel_to_pos(m_press_pos, m_lay));
    // std::cout << "picture::mouseMoveEvent. updating\n";
    update();
  }
}

void picture::mouseReleaseEvent(QMouseEvent* event) {
  QPointF delta = event->position() - m_press_pos;
  if (delta == QPointF(0, 0)) {
    // std::cout << "picture::mouseReleaseEvent. do nothing\n";
    return;
  }
  double px = delta.x() / m_lay.m_img_width;
  double py = delta.y() / m_lay.m_img_height;
  double lenx = m_lay.m_max_x - m_lay.m_min_x;
  double leny = m_lay.m_max_y - m_lay.m_min_y;
  m_lay.m_min_x -= lenx * px;
  m_lay.m_max_x -= lenx * px;
  m_lay.m_min_y -= leny * py;
  m_lay.m_max_y -= leny * py;
  emit mouse_pos_changed(pixel_to_pos(m_press_pos, m_lay));
  // std::cout << "picture::mouseReleaseEvent. updating\n";
  m_image_pos.setX(static_cast<int>(delta.x()));
  m_image_pos.setY(static_cast<int>(delta.y()));
  update();
  emit_render_signal("mouseReleaseEvent");
}

void picture::closeEvent(QCloseEvent* event) {
  emit_stop_signal("closeEvent");
  event->accept();
}

void picture::zoom_picture(double power) {
  constexpr static double zoom_coef = 0.85;
  double zoom_val = pow(zoom_coef, power);
  m_lay.m_min_x *= zoom_val;
  m_lay.m_max_x *= zoom_val;
  m_lay.m_min_y *= zoom_val;
  m_lay.m_max_y *= zoom_val;
  m_lay.m_scale *= zoom_val;
  // std::cout << "picture::zoom_picture. updating\n";
  update();
  emit_render_signal("zoom_picture");
}