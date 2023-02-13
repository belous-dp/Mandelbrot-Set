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

  m_style_menu = new QMenu(tr("&Coloring"), this);
  struct qact {
    std::string m_name;
    std::string m_tip;
    coloring m_style;
    void (picture::*slot)();
  };
  std::vector<qact> qacts = {
      {"&Binary",       "Binary coloring",     coloring::binary,       &picture::style1},
      {"&Fiery",        "Simple red coloring", coloring::fiery,        &picture::style2},
      {"&Blue",         "Kinda blue coloring", coloring::blue,         &picture::style3},
      {"&Wavy",         "16-colors palette",   coloring::wavy,         &picture::style4},
      {"&Another blue", "Less purple now",     coloring::another_blue, &picture::style5}};
  for (std::size_t i = 0; i < qacts.size(); ++i) {
    qact const& act = qacts[i];
    QAction* style_act = new QAction(tr(act.m_name.c_str()), this);
    style_act->setShortcut(QKeySequence(Qt::Key_1 + i));
    style_act->setStatusTip(tr(act.m_tip.c_str()));
    connect(style_act, &QAction::triggered, m_picture, act.slot);
    m_style_menu->addAction(style_act);
  }

  menuBar()->addMenu(m_style_menu);

  // === configuring status bar ===

  // mouse position
  m_position_label = new QLabel(this);
  m_position_label->setText(tr("Position: X=0.0, Y=0.0"));
  m_position_label->setAlignment(Qt::AlignLeft);
  statusBar()->addPermanentWidget(m_position_label);
  connect(m_picture, &picture::mouse_pos_changed, this, &window::mouse_changed);

  QLabel* spacer = new QLabel(this);
  statusBar()->addPermanentWidget(spacer, 1);

  // information about image
  m_img_info_label = new QLabel(this);
  m_img_info_label->setText(tr("Image size: minX=0.0, maxX=0.0, minY=0.0, maxY=0.0"));
  m_img_info_label->setAlignment(Qt::AlignRight);
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
  ss << "Image size: minX=" << lay.m_min.x() << ", maxX=" << lay.m_max.x()
     << ", minY=" << lay.m_min.y() << ", maxY=" << lay.m_max.y();
  m_img_info_label->setText(tr(ss.str().c_str()));
}
