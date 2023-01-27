#include "main_window.h"
#include "picture.h"
// #include <QMenuBar>
#include <QLabel>
#include <QStatusBar>
#include <QWidget>
#include <iostream>
#include <QCloseEvent>

window::window(QWidget* parent) : QMainWindow(parent) {
  std::cout << std::fixed << std::setprecision(3);
  resize(300, 200);

  m_picture = new picture(this);
  setCentralWidget(m_picture);

  // menuBar()->addMenu(tr("File"));

  statusBar()->setStyleSheet("background-color: black;");
  std::string qlabel_style_sheet = "QLabel { background-color: black; color: white; font-size: 13pt; }";

  m_position_label = new QLabel(this);
  m_position_label->setText(tr("Position: X=0.0, Y=0.0"));
  m_position_label->setAlignment(Qt::AlignLeft);
  m_position_label->setStyleSheet(tr(qlabel_style_sheet.c_str()));
  statusBar()->addPermanentWidget(m_position_label);
  connect(m_picture, &picture::mouse_pos_changed, this, &window::mouse_changed);

  QLabel* spacer = new QLabel(this);
  statusBar()->addPermanentWidget(spacer, 1);

  m_img_info_label = new QLabel(this);
  m_img_info_label->setText(tr("Image size: minX=0.0, maxX=0.0, minY=0.0, maxY=0.0"));
  m_img_info_label->setAlignment(Qt::AlignRight);
  m_img_info_label->setStyleSheet(tr(qlabel_style_sheet.c_str()));
  statusBar()->addPermanentWidget(m_img_info_label);
  connect(m_picture, &picture::render_image, this, &window::window_changed);

  setWindowTitle(tr("Mandelbrot Set"));
}

void window::closeEvent(QCloseEvent* event) {
  m_picture->close();
  event->accept();
}

void window::mouse_changed(QPointF const& mouse_pos) {
  std::stringstream ss;
  ss << "Position: X=" << mouse_pos.x() << ", Y=" << mouse_pos.y();
  m_position_label->setText(tr(ss.str().c_str()));
}

void window::window_changed(render_layout const& lay) {
  std::stringstream ss;
  ss << "Image size: minX=" << lay.m_min_x << ", maxX=" << lay.m_max_x << ", minY=" << lay.m_min_y
     << ", maxY=" << lay.m_max_y;
  m_img_info_label->setText(tr(ss.str().c_str()));
}
