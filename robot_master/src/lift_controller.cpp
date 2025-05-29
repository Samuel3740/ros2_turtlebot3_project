/**
 * @file /src/lift_controller.cpp
 *
 * @brief Implementation for the lift controller.
 *
 * @date May 2025
 **/

#include "../include/robot_master/lift_controller.hpp"

namespace robot_master {

LiftController::LiftController(std::shared_ptr<rclcpp::Node> node) : node_(node), current_height_(0.0), current_command_(0.0) {
  // 리프트 명령 퍼블리셔 초기화
  lift_cmd_pub_ = node_->create_publisher<geometry_msgs::msg::Twist>("cmd_lift_vel", 10);

  // 리프트 높이 구독자 초기화 (실제 로봇에서 사용)
  lift_height_sub_ = node_->create_subscription<std_msgs::msg::Float32>("lift_height", 10, std::bind(&LiftController::heightCallback, this, std::placeholders::_1));

  // 시뮬레이션용 타이머 (실제 하드웨어가 없는 경우)
  sim_timer_ = node_->create_wall_timer(std::chrono::milliseconds(50), std::bind(&LiftController::updateSimulation, this));

  RCLCPP_INFO(node_->get_logger(), "리프트 컨트롤러가 초기화되었습니다");
}

LiftController::~LiftController() {
  // 컨트롤러 소멸 시 모터 정지
  stop();
}

void LiftController::moveUp() {
  if (current_height_ >= MAX_HEIGHT) {
    RCLCPP_WARN(node_->get_logger(), "리프트가 최대 높이에 도달했습니다 (%.2f m)", MAX_HEIGHT);
    stop();
    return;
  }

  RCLCPP_INFO(node_->get_logger(), "리프트 올림");
  current_command_ = LIFT_SPEED;
  publishLiftCommand(LIFT_SPEED);
}

void LiftController::moveDown() {
  if (current_height_ <= MIN_HEIGHT) {
    RCLCPP_WARN(node_->get_logger(), "리프트가 최소 높이에 도달했습니다 (%.2f m)", MIN_HEIGHT);
    stop();
    return;
  }

  RCLCPP_INFO(node_->get_logger(), "리프트 내림");
  current_command_ = -LIFT_SPEED;
  publishLiftCommand(-LIFT_SPEED);
}

void LiftController::stop() {
  RCLCPP_INFO(node_->get_logger(), "리프트 정지");
  current_command_ = 0.0;
  publishLiftCommand(0.0);
}

void LiftController::publishLiftCommand(double velocity) {
  auto msg = std::make_unique<geometry_msgs::msg::Twist>();
  msg->linear.x = 0.0;
  msg->linear.y = 0.0;
  msg->linear.z = velocity;  // Z축을 리프트 움직임에 사용
  msg->angular.x = 0.0;
  msg->angular.y = 0.0;
  msg->angular.z = 0.0;

  lift_cmd_pub_->publish(std::move(msg));
}

void LiftController::heightCallback(const std_msgs::msg::Float32::SharedPtr msg) {
  // 실제 로봇에서 높이 값 업데이트
  current_height_ = msg->data;

  // 한계값 확인
  if (current_height_ >= MAX_HEIGHT && current_command_ > 0) {
    stop();
    RCLCPP_INFO(node_->get_logger(), "리프트가 최대 높이에 도달했습니다");
  } else if (current_height_ <= MIN_HEIGHT && current_command_ < 0) {
    stop();
    RCLCPP_INFO(node_->get_logger(), "리프트가 최소 높이에 도달했습니다");
  }
}

void LiftController::updateSimulation() {
  // 실제 하드웨어가 없는 경우 시뮬레이션 업데이트
  if (current_command_ != 0.0) {
    // 명령에 따라 높이 업데이트 (50ms마다 호출)
    current_height_ += current_command_ * 0.05;  // 속도 * 시간 = 거리

    // 한계값 확인
    if (current_height_ > MAX_HEIGHT) {
      current_height_ = MAX_HEIGHT;
      stop();
      RCLCPP_DEBUG(node_->get_logger(), "시뮬레이션 리프트가 최대 높이에 도달했습니다");
    } else if (current_height_ < MIN_HEIGHT) {
      current_height_ = MIN_HEIGHT;
      stop();
      RCLCPP_DEBUG(node_->get_logger(), "시뮬레이션 리프트가 최소 높이에 도달했습니다");
    }

    // 높이 정보를 ROS 로그로 출력 (디버그용)
    RCLCPP_DEBUG(node_->get_logger(), "현재 리프트 높이: %.2f m", current_height_);
  }
}

}  // namespace robot_master