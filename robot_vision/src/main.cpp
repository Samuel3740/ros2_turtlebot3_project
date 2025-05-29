#include <QApplication>
#include <iostream>

#include "../include/robot_vision/main_window.hpp"
using namespace robot_vision;
int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
