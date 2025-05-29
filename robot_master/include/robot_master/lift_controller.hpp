/**
 * @file /include/robot_master/lift_controller.hpp
 *
 * @brief Header for the lift controller.
 *
 * @date May 2025
 **/

#ifndef ROBOT_MASTER_LIFT_CONTROLLER_HPP
#define ROBOT_MASTER_LIFT_CONTROLLER_HPP

#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

namespace robot_master {

class LiftController {
 public:
  explicit LiftController(std::shared_ptr<rclcpp::Node> node);
  ~LiftController();

  // 단순 제어 메서드
  void moveUp();
  void moveDown();
  void stop();

  // 현재 높이 조회
  double getCurrentHeight() const { return current_height_; }

 private:
  // 상수
  static constexpr double LIFT_SPEED = 0.3;  // m/s
  static constexpr double MAX_HEIGHT = 0.5;  // 최대 높이 (미터)
  static constexpr double MIN_HEIGHT = 0.0;  // 최소 높이 (미터)

  // 노드 포인터
  std::shared_ptr<rclcpp::Node> node_;

  // 리프트 명령 퍼블리셔
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr lift_cmd_pub_;

  // 리프트 높이 구독자
  rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr lift_height_sub_;

  // 현재 높이 및 명령 상태
  double current_height_ = 0.0;
  double current_command_ = 0.0;

  // 시뮬레이션용 타이머
  rclcpp::TimerBase::SharedPtr sim_timer_;

  // 헬퍼 메서드
  void publishLiftCommand(double velocity);

  // 높이 구독 콜백
  void heightCallback(const std_msgs::msg::Float32::SharedPtr msg);

  // 시뮬레이션 업데이트 (실제 하드웨어가 없는 경우)
  void updateSimulation();
};

}  // namespace robot_master

#endif  // ROBOT_MASTER_LIFT_CONTROLLER_HPP