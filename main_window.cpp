#include "main_window.h"
#include "picture.h"
#include <QApplication>
#include <QLayout>
#include <QMenuBar>
#include <QPushButton>
#include <QTextBrowser>
#include <QWidget>
#include <iostream>

window::window(QWidget* parent) : QMainWindow(parent) {
  std::cout << std::fixed << std::setprecision(3);
  resize(200, 200);
  // setFixedSize(1000, 500);

  // m_button = new QPushButton("push me", this);
  // m_button->setGeometry(10, 10, 80, 30);

  m_picture = new picture(this);
  setCentralWidget(m_picture);
  
  // menuBar()->addMenu(tr("File"));

  // m_wid = new QWidget(this);
  // m_wid->setGeometry(120, 0, 50, 50);
  // QPalette pal = QPalette();
  // pal.setColor(QPalette::Window, Qt::black);
  // m_wid->setPalette(pal);

  /*m_text_browser = new QTextBrowser(this);
  m_text_browser->setGeometry(100, 100, 100, 100);*/

  /*QPalette pal = m_button->palette();
  pal.setColor(QPalette::ButtonText, QColor(Qt::blue));
  m_button->setAutoFillBackground(true);
  m_button->setPalette(pal);
  m_button->update();*/

  // layout()->addWidget(m_button);
  // layout()->addWidget(m_text_browser);
  // setLayout(mainLayout);
  // setWindowTitle(tr("Connecting buttons to processes.."));
  // connect(m_button, &QPushButton::clicked, QApplication::instance(), &QApplication::quit);
}
