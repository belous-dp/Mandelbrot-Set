//
// Created by Danila Belous on 13.01.2023 at 01:21.
//

#include <QApplication>
#include <QFile>
#include "main_window.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  window window;
  window.show();

  //Q_INIT_RESOURCE(darkstyle);
  //QFile f(":darkstyle.qss");
  QFile f(":qdarkstyle/dark/darkstyle.qss");
  if (!f.exists()) {
    qWarning() << "Unable to set dark stylesheet: file not found";
  } else {
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    app.setStyleSheet(ts.readAll());
  }

  return app.exec();
}
