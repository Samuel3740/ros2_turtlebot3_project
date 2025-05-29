/**
 * @file /include/robot_master/task_manager.hpp
 *
 * @brief Header for the task manager.
 *
 * @date May 2025
 **/

#ifndef ROBOT_MASTER_TASK_MANAGER_HPP
#define ROBOT_MASTER_TASK_MANAGER_HPP

#include <memory>
#include <string>

#include "lift_controller.hpp"
#include "rclcpp/rclcpp.hpp"

namespace robot_master {

// 작업 상태 열거형
enum class TaskState { IDLE, SEARCHING, NAVIGATING_TO_PARCEL, RECOGNIZING_PARCEL, LIFTING_PARCEL, DELIVERING, DROPPING_PARCEL, COMPLETED, ERROR };

class TaskManager {
 public:
  explicit TaskManager(std::shared_ptr<rclcpp::Node> node);
  ~TaskManager();

  // 작업 관리 메서드
  void startFindParcelTask(const std::string& item_id);
  void cancelCurrentTask();

  // 상태 조회
  TaskState getCurrentState() const;

  // 리프트 컨트롤러 액세스
  std::shared_ptr<LiftController> getLiftController() { return lift_controller_; }

 private:
  // 노드 포인터
  std::shared_ptr<rclcpp::Node> node_;

  // 현재 작업 상태
  TaskState current_state_;

  // 리프트 컨트롤러
  std::shared_ptr<LiftController> lift_controller_;

  // 현재 목표 아이템 ID
  std::string target_item_id_;

  // 작업 실행용 타이머
  rclcpp::TimerBase::SharedPtr task_timer_;

  // 작업 처리 메서드
  void processTask();
};

}  // namespace robot_master

#endif  // ROBOT_MASTER_TASK_MANAGER_HPP