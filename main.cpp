//
// Created by Danila Belous on 13.01.2023 at 01:21.
//

#include <QApplication>
#include "main_window.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);

  window window;
  window.show();

  return a.exec();
}
