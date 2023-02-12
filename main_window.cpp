#include "main_window.h"
#include "picture.h"
#include <QCloseEvent>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>
#include <iostream>

window::window(QWidget* parent) : QMainWindow(parent) {
  std::cout << std::fixed << std::setprecision(3);

  // === configuring picture ===

  resize(900, 600);

  m_picture = new picture(this);
  setCentralWidget(m_picture);

  // === configuring menus ===

  //menuBar()->setStyleSheet(tr("background-color: #505050; color: white;"));

  // style
  m_style_menu = new QMenu(tr("&Coloring"), this);
  //m_style_menu->setStyleSheet(tr("QMenu { background-color: #505050; color: white; }"));

  m_style1_act = new QAction(tr("&Pure"), this);
  m_style1_act->setShortcut(QKeySequence(Qt::Key_1));
  m_style1_act->setStatusTip(tr("Binary coloring"));
  connect(m_style1_act, &QAction::triggered, m_picture, &picture::style1);
  m_style_menu->addAction(m_style1_act);

  m_style2_act = new QAction(tr("&Fiery"), this);
  m_style2_act->setShortcut(QKeySequence(Qt::Key_2));
  m_style2_act->setStatusTip(tr("Simple red coloring"));
  connect(m_style2_act, &QAction::triggered, m_picture, &picture::style2);
  m_style_menu->addAction(m_style2_act);

  menuBar()->addMenu(m_style_menu);

  // === configuring status bar ===

  //statusBar()->setStyleSheet(tr("background-color: black;"));
  //std::string qlabel_style_sheet = "QLabel { background-color: black; color: white; font-size: 13pt; }";

  // mouse position
  m_position_label = new QLabel(this);
  m_position_label->setText(tr("Position: X=0.0, Y=0.0"));
  m_position_label->setAlignment(Qt::AlignLeft);
  //m_position_label->setStyleSheet(tr(qlabel_style_sheet.c_str()));
  statusBar()->addPermanentWidget(m_position_label);
  connect(m_picture, &picture::mouse_pos_changed, this, &window::mouse_changed);

  QLabel* spacer = new QLabel(this);
  statusBar()->addPermanentWidget(spacer, 1);
  
  // information about image
  m_img_info_label = new QLabel(this);
  m_img_info_label->setText(tr("Image size: minX=0.0, maxX=0.0, minY=0.0, maxY=0.0"));
  m_img_info_label->setAlignment(Qt::AlignRight);
  //m_img_info_label->setStyleSheet(tr(qlabel_style_sheet.c_str()));
  statusBar()->addPermanentWidget(m_img_info_label);
  connect(m_picture, &picture::window_changed, this, &window::window_changed);

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
  ss << "Image size: minX=" << lay.m_min.x() << ", maxX=" << lay.m_max.x() << ", minY=" << lay.m_min.y()
     << ", maxY=" << lay.m_max.y();
  m_img_info_label->setText(tr(ss.str().c_str()));
}
