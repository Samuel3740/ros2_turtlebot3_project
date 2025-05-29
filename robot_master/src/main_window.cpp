/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date May 2025
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include "../include/robot_master/main_window.hpp"

// Qt 헤더 추가
#include <QCloseEvent>
#include <QMessageBox>
#include <QScrollBar>
#include <QString>
#include <QTime>
// BURGER_MAX_LIN_VEL = 0.22
// BURGER_MAX_ANG_VEL = 2.84

namespace robot_master {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign) {
  ui->setupUi(this);

  QIcon icon("://ros-icon.png");
  this->setWindowIcon(icon);

  qnode = new QNode();

  move(820, 0);  // ui위치
  QObject::connect(qnode, SIGNAL(rosShutDown()), this, SLOT(close()));
  QObject::connect(qnode, SIGNAL(dataReceived()), this, SLOT(updateData()));
  QObject::connect(qnode, SIGNAL(logMessage(QString)), this, SLOT(appendLog(QString)));
  QObject::connect(qnode, SIGNAL(taskStateChanged(int)), this, SLOT(updateTaskState(int)));

  // 초기 로그 메시지
  appendLog("시스템이 초기화되었습니다");
}

void MainWindow::closeEvent(QCloseEvent* event) { QMainWindow::closeEvent(event); }

MainWindow::~MainWindow() { delete ui; }

/*
 * UI 업데이트
 * 모터 상태, 리프트 높이, 작업 상태를 업데이트
 * 모터 상태는 linear.x와 angular.z로 표시
 * 리프트 높이는 0.4m 이상일 때 녹색, 0.2m 이상일 때 노란색으로 표시
 */

void MainWindow::updateData() {
  // 모터 상태 업데이트
  ui->label_linear_x->setText(QString::number(qnode->driving_.motor_value_.linear.x));
  ui->label_angular_z->setText(QString::number(qnode->driving_.motor_value_.angular.z));

  // 리프트 상태 업데이트
  if (qnode->task_manager_) {
    auto lift_controller = qnode->task_manager_->getLiftController();
    if (lift_controller) {
      double height = lift_controller->getCurrentHeight();
      ui->liftHeightValue->setText(QString::number(height, 'f', 2));

      // 리프트 높이에 따른 색상 변경 (옵션)
      if (height > 0.4) {
        ui->liftHeightValue->setStyleSheet("color: #a3be8c;");  // 녹색
      } else if (height > 0.2) {
        ui->liftHeightValue->setStyleSheet("color: #ebcb8b;");  // 노랑
      } else {
        ui->liftHeightValue->setStyleSheet("color: #d8dee9;");  // 기본
      }
    }
  }
}

/*
 * 로그 메시지 추가
 * 로그 메시지를 UI의 텍스트 편집기에 추가
 * 현재 시간과 함께 메시지를 표시
 */

void MainWindow::appendLog(const QString& message) {
  QString current_time = QTime::currentTime().toString("hh:mm:ss");
  QString log_entry = QString("[%1] %2").arg(current_time).arg(message);

  ui->textEdit_debugLog->append(log_entry);

  // 스크롤을 항상 아래로 유지
  ui->textEdit_debugLog->verticalScrollBar()->setValue(ui->textEdit_debugLog->verticalScrollBar()->maximum());
}

/*
 * 상태 업데이트
 * 상태에 따라 UI를 업데이트
 * 상태는 TaskState enum을 사용하여 정의
 * IDLE, SEARCHING, NAVIGATING_TO_PARCEL, RECOGNIZING_PARCEL,
 * LIFTING_PARCEL, DELIVERING, DROPPING_PARCEL, COMPLETED, ERROR
 */

void MainWindow::updateTaskState(int state) {
  TaskState task_state = static_cast<TaskState>(state);

  // 상태에 따른 UI 업데이트
  switch (task_state) {
    case TaskState::IDLE:
      ui->statusValueLabel->setText("대기 중");
      ui->statusValueLabel->setStyleSheet("color: #d8dee9;");
      break;
    case TaskState::SEARCHING:
      ui->statusValueLabel->setText("위치 검색 중");
      ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
      break;
    case TaskState::NAVIGATING_TO_PARCEL:
      ui->statusValueLabel->setText("물품 이동 중");
      ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
      break;
    case TaskState::RECOGNIZING_PARCEL:
      ui->statusValueLabel->setText("물품 인식 중");
      ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
      break;
    case TaskState::LIFTING_PARCEL:
      ui->statusValueLabel->setText("물품 들어 올리는 중");
      ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
      break;
    case TaskState::DELIVERING:
      ui->statusValueLabel->setText("배송 중");
      ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
      break;
    case TaskState::DROPPING_PARCEL:
      ui->statusValueLabel->setText("물품 내려놓는 중");
      ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
      break;
    case TaskState::COMPLETED:
      ui->statusValueLabel->setText("작업 완료");
      ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
      break;
    case TaskState::ERROR:
      ui->statusValueLabel->setText("오류 발생");
      ui->statusValueLabel->setStyleSheet("color: #bf616a;");
      break;
  }
}

/*
 * 로봇 조종 핸들러
 * 방향 버튼을 누르면 로봇이 해당 방향으로 이동
 */

// 방향 버튼 핸들러 구현
void MainWindow::on_forwardButton_pressed() {
  // RobotDriving::start를 true로 설정하여 로봇이 동작하도록 함
  RobotDriving::start = true;

  // setSpeed 메서드 사용
  qnode->driving_.setSpeed(0.1, 0.0);  // 전진 속도

  // UI 업데이트
  ui->statusValueLabel->setText("이동 중: 전진");
  ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
  ui->label_linear_x->setText(QString::number(qnode->driving_.motor_value_.linear.x));
  ui->label_angular_z->setText(QString::number(qnode->driving_.motor_value_.angular.z));

  appendLog("전진 명령 실행");
}

void MainWindow::on_backwardButton_pressed() {
  RobotDriving::start = true;

  // setSpeed 메서드 사용
  qnode->driving_.setSpeed(-0.1, 0.0);  // 후진 속도

  ui->statusValueLabel->setText("이동 중: 후진");
  ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
  ui->label_linear_x->setText(QString::number(qnode->driving_.motor_value_.linear.x));
  ui->label_angular_z->setText(QString::number(qnode->driving_.motor_value_.angular.z));

  appendLog("후진 명령 실행");
}

void MainWindow::on_leftButton_pressed() {
  RobotDriving::start = true;

  // setSpeed 메서드 사용
  qnode->driving_.setSpeed(0.0, 1.0);  // 좌회전 속도

  ui->statusValueLabel->setText("이동 중: 좌회전");
  ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
  ui->label_linear_x->setText(QString::number(qnode->driving_.motor_value_.linear.x));
  ui->label_angular_z->setText(QString::number(qnode->driving_.motor_value_.angular.z));

  appendLog("좌회전 명령 실행");
}

void MainWindow::on_rightButton_pressed() {
  RobotDriving::start = true;

  // setSpeed 메서드 사용
  qnode->driving_.setSpeed(0.0, -1.0);  // 우회전 속도

  ui->statusValueLabel->setText("이동 중: 우회전");
  ui->statusValueLabel->setStyleSheet("color: #a3be8c;");
  ui->label_linear_x->setText(QString::number(qnode->driving_.motor_value_.linear.x));
  ui->label_angular_z->setText(QString::number(qnode->driving_.motor_value_.angular.z));

  appendLog("우회전 명령 실행");
}

void MainWindow::on_stopButton_clicked() {
  // RobotDriving::start를 false로 설정하여 go() 함수에서 로봇이 멈추도록 함
  RobotDriving::start = false;

  // setSpeed 메서드 사용
  qnode->driving_.setSpeed(0.0, 0.0);

  ui->statusValueLabel->setText("정지");
  ui->statusValueLabel->setStyleSheet("color: #d8dee9;");
  ui->label_linear_x->setText("0.0");
  ui->label_angular_z->setText("0.0");

  appendLog("정지 명령 실행");
}

void MainWindow::on_emergencyStopButton_clicked() {
  // 긴급 정지: 모든 동작 중지
  RobotDriving::start = false;

  // setSpeed 메서드 사용
  qnode->driving_.setSpeed(0.0, 0.0);

  // 화면 표시 업데이트
  ui->statusValueLabel->setText("비상 정지");
  ui->statusValueLabel->setStyleSheet("color: #bf616a;");  // 빨간색으로 상태 표시
  ui->label_linear_x->setText("0.0");
  ui->label_angular_z->setText("0.0");

  // 현재 작업 취소
  qnode->cancelTask();

  appendLog("비상 정지 명령 실행");
}

/*
 * 리프트 버튼 핸들러
 * 리프트 버튼을 누르면 리프트가 올라가고, 떼면 멈춤
 * 리프트 버튼을 누르면 리프트가 내려가고, 떼면 멈춤
 */

void MainWindow::on_liftUpButton_pressed() {
  if (qnode->task_manager_) {
    auto lift_controller = qnode->task_manager_->getLiftController();
    if (lift_controller) {
      lift_controller->moveUp();
      appendLog("리프트 올림 명령 실행");
    } else {
      appendLog("리프트 컨트롤러를 초기화할 수 없습니다");
    }
  } else {
    appendLog("작업 관리자가 초기화되지 않았습니다");
  }
}

void MainWindow::on_liftDownButton_pressed() {
  if (qnode->task_manager_) {
    auto lift_controller = qnode->task_manager_->getLiftController();
    if (lift_controller) {
      lift_controller->moveDown();
      appendLog("리프트 내림 명령 실행");
    } else {
      appendLog("리프트 컨트롤러를 초기화할 수 없습니다");
    }
  } else {
    appendLog("작업 관리자가 초기화되지 않았습니다");
  }
}

void MainWindow::on_liftUpButton_released() {
  if (qnode->task_manager_) {
    auto lift_controller = qnode->task_manager_->getLiftController();
    if (lift_controller) {
      lift_controller->stop();
      appendLog("리프트 정지");
    }
  }
}

void MainWindow::on_liftDownButton_released() {
  if (qnode->task_manager_) {
    auto lift_controller = qnode->task_manager_->getLiftController();
    if (lift_controller) {
      lift_controller->stop();
      appendLog("리프트 정지");
    }
  }
}

/*
 * 물품 검색 버튼 핸들러
 * 물품 검색 버튼을 누르면 물품 검색 작업 시작
 * 물품 정보는 lineEdit_parcelInfo에서 가져옴
 */

void MainWindow::on_pushButton_findParcel_clicked() {
  QString parcelInfo = ui->lineEdit_parcelInfo->text();

  if (parcelInfo.trimmed().isEmpty()) {
    ui->label_parcelResult->setText("택배 정보를 입력하세요.");
    appendLog("오류: 빈 택배 정보");
    return;
  }

  QString resultMsg = "입력한 택배 정보: " + parcelInfo;
  ui->label_parcelResult->setText(resultMsg);

  qnode->setItemInfo(parcelInfo.toStdString());
  qnode->startFindParcelTask();

  appendLog("물품 검색 작업 시작: " + parcelInfo);
}

}  // namespace robot_master