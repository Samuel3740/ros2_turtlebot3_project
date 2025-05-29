/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date January 2025
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include "../include/robot_vision/main_window.hpp"
namespace robot_vision {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign) {
  ui->setupUi(this);

  QIcon icon("://ros-icon.png");
  this->setWindowIcon(icon);

  qnode = new QNode();

  move(820, 330);
  QObject::connect(qnode, &QNode::rosShutDown, this, &MainWindow::close);
  QObject::connect(qnode, &QNode::sigRcvImg, this, &MainWindow::slotUpdateImg);
}

void MainWindow::closeEvent(QCloseEvent* event) { QMainWindow::closeEvent(event); }

MainWindow::~MainWindow() { delete ui; }

void MainWindow::slotUpdateImg() {
  clone_mat = qnode->imgRaw->clone();                                           // 원본 이미지 복사
  cv::resize(clone_mat, clone_mat, cv::Size(640, 360), 0, 0, cv::INTER_CUBIC);  // 이미지 크기 조정

  QImage RGB_im((const unsigned char*)(clone_mat.data), clone_mat.cols, clone_mat.rows, QImage::Format_RGB888);
  ui->label->setPixmap(QPixmap::fromImage(RGB_im));

  delete qnode->imgRaw;  // 동적 할당된 원본 이미지 메모리 해제
  qnode->imgRaw = NULL;
  qnode->isreceived = false;  // 이미지 수신 플래그 재설정
}

}  // namespace robot_vision