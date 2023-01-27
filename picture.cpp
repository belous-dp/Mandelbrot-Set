//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

#include "picture.h"
#include <QPainter>
#include <iostream>

picture::picture(QWidget* parent) : QWidget(parent) {
  // QPalette pal = QPalette();
  // pal.setColor(QPalette::Window, Qt::black);
  // setAutoFillBackground(true);
  // setPalette(pal);

  m_workers.moveToThread(&m_workers_thread);
  connect(this, &picture::render_image, &m_workers, &workers::render_image);
  connect(&m_workers, &workers::image_ready, this, &picture::image_ready);
  m_workers_thread.start();
}

picture::~picture() {
  m_workers_thread.quit();
  m_workers_thread.wait();
}

void picture::resizeEvent(QResizeEvent* event) {
  emit render_image(width(), height());
  std::cout << "sent render call for w=" << width() << ", h=" << height() << std::endl;
}

void picture::paintEvent(QPaintEvent* event) {
  QPainter p(this);
  std::cout << "paint event, w=" << width() << ", h=" << height() << std::endl;
  if (m_image.isNull()) {
    p.drawText(rect(), Qt::AlignBottom, tr("initial rendering..."));
  } else {
    // можно было бы прямо здесь вызывать invokeMethod у workers,
    // но иногда нам не требуется заново рендерить картинку,
    // поэтому ответственность за запросы о рендеринге можно возложить
    // на те функции, в которых это действительно нужно
    p.fillRect(rect(), Qt::black);
    p.drawImage(0, 0, m_image);
  }
}

void picture::image_ready(QImage const& image) {
  m_image = image;
  update();
}
