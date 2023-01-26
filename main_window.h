//
// Created by Danila Belous on 26.01.2023 at 11:37.
//

#pragma once

#include <QMainWindow>

class QPushButton; // forward declare to reduce the header size
class QWidget;
class picture;
class QTextBrowser;

class window : public QMainWindow {
  Q_OBJECT

public:
  explicit window(QWidget* parent = nullptr);

private:
  //QPushButton* m_button;
  picture* m_picture;
  //QWidget* m_wid;
  //QTextBrowser* m_text_browser;
};
