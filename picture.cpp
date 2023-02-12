//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

#include "picture.h"
#include <QPainter>
#include <QResizeEvent>
#include <QSinglePointEvent>
#include <QWheelEvent>
#include <iostream>

picture::picture(QWidget* parent) : QWidget(parent) {

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
  std::cout << "picture::image_ready. updating (scheduling repaint)\n";
  update();
}

void picture::emit_render_signal(std::string from) {
  std::cout << "picture::emit_render_signal from function " + from + '\n';
  emit render_image(m_lay);
  m_workers.m_max_version++;
}
void picture::emit_render_signal() {
  emit render_image(m_lay);
  m_workers.m_max_version++;
}

void picture::emit_stop_signal(std::string from) {
  std::cout << "picture::emit_stop_signal from function " + from + '\n';
  emit render_image({});
  m_workers.m_max_version++;
}
void picture::emit_stop_signal() {
  std::cout << "picture::emit_stop_signal\n";
  emit render_image({});
  m_workers.m_max_version++;
}

void picture::resizeEvent(QResizeEvent* event) {
  std::cout << "picture::resizeEvent: ";
  if (event->oldSize().width() != width() || event->oldSize().height() != height()) {
    std::cout << "new size\n";
    reset_layout();
    emit_render_signal("resizeEvent");
  } else {
    std::cout << "same size\n";
  }
}

void picture::reset_layout() {
  m_image_scale = 1.;
  m_lay.m_min_x = INIT_MIN_X;
  m_lay.m_max_x = INIT_MAX_X;
  m_lay.m_min_y = INIT_MIN_Y;
  m_lay.m_max_y = INIT_MAX_Y;
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
  std::cout << "picture::paintEvent: ";
  if (m_image.isNull()) {
    p.setPen(Qt::white);
    p.drawText(rect(), Qt::AlignCenter, tr("Initial rendering..."));
  } else if (!m_image_delta.isNull() || !m_image.offset().isNull()) {
    std::cout << "shifting, x=" << m_image.offset().x() + m_image_delta.x()
              << ", y=" << m_image.offset().y() + m_image_delta.y() << ", scale=" << m_image_scale << '\n';
    p.drawImage(m_image.offset().x() + m_image_delta.x(), m_image.offset().y() + m_image_delta.y(), m_image);
  } else {
    std::cout << "scaling, x=" << m_image.offset().x() + m_image_delta.x()
              << ", y=" << m_image.offset().y() + m_image_delta.y() << ", scale=" << m_image_scale << '\n';
    // the image itself can be scaled using `QImage.scaled()`, but that method would return
    // a copy of the image
    // so it'd be better to scale the coordinate system without copying the image
    // note: p.save() & p.restore() aren't needed cuz QPainter is resetted each paint event
    p.scale(1 / m_image_scale, 1 / m_image_scale);
    p.drawImage(0, 0, m_image);
    m_image_scale = 1.;
  }
}

void picture::wheelEvent(QWheelEvent* event) {
  std::cout << "picture::wheelEvent\n";
  update_mouse(event);
  int degrees = event->angleDelta().y() / 8;
  double steps = degrees / 15.;
  zoom_picture(steps);
}

void picture::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    std::cout << "picture::mousePressEvent\n";
    update_mouse(event);
  }
}

void picture::update_mouse(QSinglePointEvent const* event) {
  emit_stop_signal();
  m_mouse_press_ppos = event->position();
  //emit mouse_pos_changed(pixel_to_pos(m_mouse_press_ppos, m_lay));
}

void picture::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() & Qt::LeftButton) {
    m_image_delta = (event->position() - m_mouse_press_ppos).toPoint();
    //emit mouse_pos_changed(pixel_to_pos(m_mouse_press_ppos + delta, m_lay));
    std::cout << "picture::mouseMoveEvent. updating\n";
    update();
  }
}

void picture::mouseReleaseEvent(QMouseEvent* event) {
  m_image_delta = (event->position() - m_mouse_press_ppos).toPoint();
  if (m_image_delta == QPoint(0, 0)) {
    std::cout << "picture::mouseReleaseEvent. do nothing\n";
    return;
  }
  //emit mouse_pos_changed(pixel_to_pos(m_mouse_press_ppos + delta, m_lay));
  double px = static_cast<double>(m_image_delta.x()) / m_lay.m_img_width;
  double py = static_cast<double>(m_image_delta.y()) / m_lay.m_img_height;
  double lenx = m_lay.m_max_x - m_lay.m_min_x;
  double leny = m_lay.m_max_y - m_lay.m_min_y;
  m_lay.m_min_x -= lenx * px;
  m_lay.m_max_x -= lenx * px;
  m_lay.m_min_y -= leny * py;
  m_lay.m_max_y -= leny * py;

  m_image.setOffset(m_image.offset() + m_image_delta);
  m_image_delta = {0, 0};
  std::cout << "picture::mouseReleaseEvent. updating\n";
  update();
  emit_render_signal("mouseReleaseEvent");
}

void picture::closeEvent(QCloseEvent* event) {
  emit_stop_signal("closeEvent");
  event->accept();
}
/**
 * zooms the picture the way that preserves relative cursor position
 * in other words, zooms into the cursor
 */
void picture::zoom_picture(double power) {
  QPointF old_mouse_pos = pixel_to_pos(m_mouse_press_ppos, m_lay);
  constexpr static double zoom_coef = 0.85;
  double zoom_val = pow(zoom_coef, power);
  m_image_scale *= zoom_val;
  m_lay.m_min_x *= zoom_val;
  m_lay.m_max_x *= zoom_val;
  m_lay.m_min_y *= zoom_val;
  m_lay.m_max_y *= zoom_val;
  QPointF shift = old_mouse_pos - pixel_to_pos(m_mouse_press_ppos, m_lay);
  m_lay.m_min_x += shift.x();
  m_lay.m_max_x += shift.x();
  m_lay.m_min_y += shift.y();
  m_lay.m_max_y += shift.y();
  std::cout << "picture::zoom_picture. updating\n";
  update(); // here 'repaint()' can be called,
  // thus line "m_lay.m_scale = 1.;" in 'emit_render_signal()' can be removed.
  // but i think 'update()' is sligthly better than 'repaint()'
  // in terms of performance
  emit_render_signal("zoom_picture");
}