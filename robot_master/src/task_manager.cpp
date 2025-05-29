/**
 * @file /src/task_manager.cpp
 *
 * @brief Implementation for the task manager.
 *
 * @date May 2025
 **/

#include "../include/robot_master/task_manager.hpp"

namespace robot_master {

TaskManager::TaskManager(std::shared_ptr<rclcpp::Node> node) : node_(node), current_state_(TaskState::IDLE), target_item_id_("") {
  // 리프트 컨트롤러 초기화
  lift_controller_ = std::make_shared<LiftController>(node_);

  // 작업 처리 타이머 설정 (100ms 간격)
  task_timer_ = node_->create_wall_timer(std::chrono::milliseconds(100), std::bind(&TaskManager::processTask, this));

  RCLCPP_INFO(node_->get_logger(), "작업 관리자가 초기화되었습니다");
}

TaskManager::~TaskManager() { cancelCurrentTask(); }

void TaskManager::startFindParcelTask(const std::string& item_id) {
  if (current_state_ != TaskState::IDLE && current_state_ != TaskState::COMPLETED && current_state_ != TaskState::ERROR) {
    RCLCPP_WARN(node_->get_logger(), "다른 작업이 실행 중일 때는 새 작업을 시작할 수 없습니다");
    return;
  }

  target_item_id_ = item_id;
  current_state_ = TaskState::SEARCHING;

  RCLCPP_INFO(node_->get_logger(), "물품 검색 작업을 시작합니다: %s", item_id.c_str());
}

void TaskManager::cancelCurrentTask() {
  if (current_state_ == TaskState::IDLE) {
    return;
  }

  // 리프트 정지
  lift_controller_->stop();

  // 작업 취소 및 상태 초기화
  current_state_ = TaskState::IDLE;
  target_item_id_ = "";

  RCLCPP_INFO(node_->get_logger(), "작업이 취소되었습니다");
}

TaskState TaskManager::getCurrentState() const { return current_state_; }

void TaskManager::processTask() {
  // 현재 상태에 따른 작업 처리
  switch (current_state_) {
    case TaskState::IDLE:
      // 대기 상태, 작업 없음
      break;

    case TaskState::SEARCHING:
      // 물품 위치 검색 로직 (여기서는 간단한 시뮬레이션)
      RCLCPP_DEBUG(node_->get_logger(), "물품을 검색 중입니다: %s", target_item_id_.c_str());

      // 테스트용: 5초 후 다음 단계로 진행
      static int search_counter = 0;
      if (++search_counter >= 50) {  // 100ms * 50 = 5초
        current_state_ = TaskState::NAVIGATING_TO_PARCEL;
        search_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "물품 위치를 찾았습니다. 이동 중...");
      }
      break;

    case TaskState::NAVIGATING_TO_PARCEL:
      // 물품으로 이동 로직
      static int nav_counter = 0;
      if (++nav_counter >= 30) {  // 3초
        current_state_ = TaskState::RECOGNIZING_PARCEL;
        nav_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "물품 위치에 도착했습니다. 물품 인식 중...");
      }
      break;

    case TaskState::RECOGNIZING_PARCEL:
      // QR 코드 인식 등의 로직
      static int recog_counter = 0;
      if (++recog_counter >= 20) {  // 2초
        current_state_ = TaskState::LIFTING_PARCEL;
        recog_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "물품을 인식했습니다. 들어올리는 중...");
      }
      break;

    case TaskState::LIFTING_PARCEL:
      // 리프트 올리기
      lift_controller_->moveUp();

      static int lift_counter = 0;
      if (++lift_counter >= 20) {  // 2초
        lift_controller_->stop();
        current_state_ = TaskState::DELIVERING;
        lift_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "물품을 들어올렸습니다. 목적지로 이동 중...");
      }
      break;

    case TaskState::DELIVERING:
      // 목적지로 이동
      static int deliver_counter = 0;
      if (++deliver_counter >= 30) {  // 3초
        current_state_ = TaskState::DROPPING_PARCEL;
        deliver_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "목적지에 도착했습니다. 물품을 내려놓는 중...");
      }
      break;

    case TaskState::DROPPING_PARCEL:
      // 리프트 내리기
      lift_controller_->moveDown();

      static int drop_counter = 0;
      if (++drop_counter >= 20) {  // 2초
        lift_controller_->stop();
        current_state_ = TaskState::COMPLETED;
        drop_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "물품 배송이 완료되었습니다");
      }
      break;

    case TaskState::COMPLETED:
      // 작업 완료, 대기 상태로 전환
      static int complete_counter = 0;
      if (++complete_counter >= 50) {  // 5초
        current_state_ = TaskState::IDLE;
        complete_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "작업이 완료되었습니다. 대기 상태로 돌아갑니다");
      }
      break;

    case TaskState::ERROR:
      // 오류 상태 처리
      static int error_counter = 0;
      if (++error_counter >= 30) {  // 3초
        current_state_ = TaskState::IDLE;
        error_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "오류 상태가 해제되었습니다. 대기 상태로 돌아갑니다");
      }
      break;
  }
}

}  // namespace robot_master