/**
 * @file /include/robot_vision/main_window.hpp
 *
 * @brief Qt based gui for %(package)s.
 *
 * @date January 2025
 **/

#ifndef robot_vision_MAIN_WINDOW_H
#define robot_vision_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QMainWindow>

#include "QIcon"
#include "qnode.hpp"
#include "ui_mainwindow.h"

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
namespace robot_vision {
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  QNode* qnode;
  cv::Mat clone_mat;

 private:
  Ui::MainWindowDesign* ui;
  void closeEvent(QCloseEvent* event);

 public Q_SLOTS:
 void slotUpdateImg();
};

}  // namespace robot_vision

#endif  // robot_vision_MAIN_WINDOW_H
