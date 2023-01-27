//
// Created by Danila Belous on 26.01.2023 at 14:08.
//

#pragma once

#include "workers.h"
#include <QThread>
#include <QWidget>

class picture : public QWidget {
  Q_OBJECT

public:
  explicit picture(QWidget* parent = nullptr);
  ~picture();

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private slots:
  void image_ready(QImage const& image);

signals:
  void render_image(int width, int height);

private:
  QThread m_workers_thread;
  workers m_workers;
  QImage m_image;
};
