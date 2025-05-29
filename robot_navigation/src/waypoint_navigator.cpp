/**
 * @file /src/waypoint_navigator.cpp
 *
 * @brief TurtleBot3 웨이포인트 네비게이션 구현 - Python 코드 직접 변환
 *
 * @date May 2025
 **/

#include "robot_navigation/waypoint_navigator.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

namespace robot_navigation {

WaypointNavigator::WaypointNavigator() : Node("waypoint_navigator"), navigation_active_(false), current_waypoint_index_(0) {
  // 네비게이션 액션 클라이언트 초기화
  navigate_client_ = rclcpp_action::create_client<NavigateAction>(this, "navigate_to_pose");

  // 로그 출력
  RCLCPP_INFO(this->get_logger(), "웨이포인트 네비게이터 노드가 시작되었습니다.");
}

void WaypointNavigator::setInitialPose(double x, double y, double z, double w) {
  // 초기 포즈 발행자 생성
  auto initial_pose_pub = this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("initialpose", 10);

  // 초기 포즈 메시지 생성
  auto initial_pose = geometry_msgs::msg::PoseWithCovarianceStamped();
  initial_pose.header.frame_id = "map";
  initial_pose.header.stamp = this->now();

  initial_pose.pose.pose.position.x = x;
  initial_pose.pose.pose.position.y = y;
  initial_pose.pose.pose.position.z = 0.0;

  initial_pose.pose.pose.orientation.x = 0.0;
  initial_pose.pose.pose.orientation.y = 0.0;
  initial_pose.pose.pose.orientation.z = z;
  initial_pose.pose.pose.orientation.w = w;

  // 초기 포즈 발행 (Nav2가 인식할 수 있도록 여러 번 발행)
  for (int i = 0; i < 5; ++i) {
    initial_pose_pub->publish(initial_pose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  RCLCPP_INFO(this->get_logger(), "초기 위치가 설정되었습니다: (%.2f, %.2f)", x, y);
}

bool WaypointNavigator::waitUntilNav2Active() {
  // Python의 waitUntilNav2Active() 함수와 유사하게 구현
  RCLCPP_INFO(this->get_logger(), "Nav2 시스템이 활성화될 때까지 대기 중...");

  // Nav2 액션 서버 대기 - 최대 10초
  if (!navigate_client_->wait_for_action_server(std::chrono::seconds(10))) {
    RCLCPP_ERROR(this->get_logger(), "Nav2 액션 서버가 활성화되지 않았습니다. 타임아웃 발생!");
    return false;
  }

  // 추가 2초 대기 (Nav2 완전 초기화를 위해)
  std::this_thread::sleep_for(std::chrono::seconds(2));
  RCLCPP_INFO(this->get_logger(), "Nav2 시스템 활성화 완료!");
  return true;
}

bool WaypointNavigator::loadWaypointsFromFile(const std::string& /* filePath */) {
  // 간소화된 구현 - 이번에는 하드코딩된 웨이포인트 사용
  setHardcodedWaypoints();
  return true;
}

void WaypointNavigator::setHardcodedWaypoints() {
  waypoints_.clear();
  location_names_.clear();

  // 이름: 총 8개
  location_names_ = {
    "시작 위치", 
    "경유 위치 A", 
    "위치 1", 
    "위치 2", 
    "위치 3", 
    "위치 4", 
    "경유 위치 A", 
    "시작 위치 (귀환)"
  };

  // 좌표: 총 8개 (위와 순서 및 개수 일치)
  std::vector<std::tuple<double, double, double>> coords = {
      std::make_tuple(0.01, 0.0, 0.0),        // 시작 위치
      std::make_tuple(0.3, 0.0, 0.0),         // 경유 위치 A (수정된 좌표)
      std::make_tuple(0.5, 0.5, M_PI / 2),    // 위치 1
      std::make_tuple(0.8, 0.5, M_PI / 2),    // 위치 2
      std::make_tuple(0.8, -0.5, -M_PI / 2),  // 위치 3
      std::make_tuple(0.5, -0.5, -M_PI / 2),  // 위치 4
      std::make_tuple(0.3, 0.0, 0.0),         // 경유 위치 A
      std::make_tuple(0.01, 0.0, 0.0)         // 시작 위치 (귀환)
  };

  // 로그와 Waypoint 배열 채우기
  for (size_t i = 0; i < coords.size(); i++) {
    Waypoint wp;
    wp.name = location_names_[i];
    wp.x = std::get<0>(coords[i]);
    wp.y = std::get<1>(coords[i]);
    wp.yaw = std::get<2>(coords[i]);
    waypoints_.push_back(wp);

    RCLCPP_INFO(this->get_logger(), "웨이포인트 %zu: %s (x=%.2f, y=%.2f, 방향=%.1f°)", 
                i + 1, wp.name.c_str(), wp.x, wp.y, wp.yaw * 180.0 / M_PI);
  }
}


void WaypointNavigator::setWaypointsManually(const std::vector<Waypoint>& waypoints) {
  waypoints_ = waypoints;

  // 이름이 없을 경우 기본 이름 생성
  location_names_.clear();
  for (size_t i = 0; i < waypoints_.size(); i++) {
    if (waypoints_[i].name.empty()) {
      location_names_.push_back("위치 " + std::to_string(i + 1));
    } else {
      location_names_.push_back(waypoints_[i].name);
    }
  }

  for (size_t i = 0; i < waypoints_.size(); i++) {
    const auto& wp = waypoints_[i];
    RCLCPP_INFO(this->get_logger(), "웨이포인트 %zu: %s (x=%.2f, y=%.2f, 방향=%.1f°)", i + 1, location_names_[i].c_str(), wp.x, wp.y, wp.yaw * 180.0 / M_PI);
  }
}

bool WaypointNavigator::navigateToWaypoints(double waitTime) {
  if (waypoints_.empty()) {
    RCLCPP_ERROR(this->get_logger(), "웨이포인트가 설정되지 않았습니다.");
    return false;
  }

  // Nav2 활성화 확인
  if (!waitUntilNav2Active()) {
    return false;
  }

  RCLCPP_INFO(this->get_logger(), "\n모든 위치를 순차적으로 방문합니다...");

  navigation_active_ = true;

  // 모든 웨이포인트 처리를 순차적으로 진행 (Python 코드와 동일하게)
  for (size_t i = 0; i < waypoints_.size(); i++) {
    const auto& waypoint = waypoints_[i];

    RCLCPP_INFO(this->get_logger(), "\n%s로 이동 중... (%zu/%zu)", location_names_[i].c_str(), i + 1, waypoints_.size());

    // 웨이포인트에서 PoseStamped 생성
    auto goal_pose = createPoseFromWaypoint(waypoint);

    // 목표 지점으로 이동 요청
    auto goal_msg = NavigateAction::Goal();
    goal_msg.pose = goal_pose;

    // 현재 웨이포인트 인덱스 설정
    current_waypoint_index_ = i;

    // 네비게이션 액션 전송
    auto send_goal_options = rclcpp_action::Client<NavigateAction>::SendGoalOptions();
    send_goal_options.goal_response_callback = std::bind(&WaypointNavigator::goalResponseCallback, this, std::placeholders::_1);
    send_goal_options.feedback_callback = std::bind(&WaypointNavigator::feedbackCallback, this, std::placeholders::_1, std::placeholders::_2);
    send_goal_options.result_callback = std::bind(&WaypointNavigator::resultCallback, this, std::placeholders::_1);

    auto goal_handle_future = navigate_client_->async_send_goal(goal_msg, send_goal_options);

    // 목표 요청 결과 대기
    if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), goal_handle_future) != rclcpp::FutureReturnCode::SUCCESS) {
      RCLCPP_ERROR(this->get_logger(), "목표 전송 실패!");
      navigation_active_ = false;
      return false;
    }

    // 비동기 goal 전송 결과가 유효한 GoalHandle을 반환하지 않았을 경우 거부
    auto goal_handle = goal_handle_future.get();
    if (!goal_handle) {
      RCLCPP_ERROR(this->get_logger(), "목표가 거부되었습니다.");
      navigation_active_ = false;
      return false;
    }

    // 작업 완료 대기 (Python의 isTaskComplete와 유사)
    auto result_future = navigate_client_->async_get_result(goal_handle);

    // 이동 중 피드백 처리 - Python과 동일하게 1초마다 업데이트
    while (rclcpp::ok()) {
      // 1초 제한으로 결과 확인
      auto wait_result = rclcpp::spin_until_future_complete(this->get_node_base_interface(), result_future, std::chrono::seconds(1));

      if (wait_result == rclcpp::FutureReturnCode::SUCCESS) {
        // 작업 완료됨
        break;
      } else if (wait_result == rclcpp::FutureReturnCode::TIMEOUT) {
        // 아직 진행 중 - 추가 처리 없음 (피드백은 콜백에서 처리)
        continue;
      } else {
        // 오류 발생
        RCLCPP_ERROR(this->get_logger(), "네비게이션 중 오류 발생!");
        navigate_client_->async_cancel_goal(goal_handle);
        navigation_active_ = false;
        return false;
      }
    }

    // 결과 확인
    auto result = result_future.get();
    auto status = result.code;

    if (status == rclcpp_action::ResultCode::SUCCEEDED) {
      RCLCPP_INFO(this->get_logger(), "%s 도착 성공!", location_names_[i].c_str());

      // 마지막 지점이 아니면 대기
      if (i < waypoints_.size() - 1) {
        RCLCPP_INFO(this->get_logger(), "%.1f초간 대기 후 다음 위치로 이동합니다...", waitTime);
        std::this_thread::sleep_for(std::chrono::duration<double>(waitTime));
      }
    } else if (status == rclcpp_action::ResultCode::CANCELED) {
      RCLCPP_ERROR(this->get_logger(), "%s 이동이 취소되었습니다.", location_names_[i].c_str());
    } else if (status == rclcpp_action::ResultCode::ABORTED) {
      RCLCPP_ERROR(this->get_logger(), "%s 이동 실패!", location_names_[i].c_str());
      RCLCPP_INFO(this->get_logger(), "다음 위치로 계속 진행합니다...");
    } else {
      RCLCPP_ERROR(this->get_logger(), "%s 이동 결과: 알 수 없는 상태", location_names_[i].c_str());
    }
  }

  RCLCPP_INFO(this->get_logger(), "\n모든 위치 방문 완료!");
  navigation_active_ = false;
  return true;
}

geometry_msgs::msg::PoseStamped WaypointNavigator::createPoseFromWaypoint(const Waypoint& waypoint) {
  geometry_msgs::msg::PoseStamped pose;
  pose.header.frame_id = "map";
  pose.header.stamp = this->now();
  pose.pose.position.x = waypoint.x;
  pose.pose.position.y = waypoint.y;
  pose.pose.position.z = 0.0;

  // Python 코드와 동일하게 쿼터니언 계산
  double cy = std::cos(waypoint.yaw * 0.5);
  double sy = std::sin(waypoint.yaw * 0.5);
  pose.pose.orientation.x = 0.0;
  pose.pose.orientation.y = 0.0;
  pose.pose.orientation.z = sy;
  pose.pose.orientation.w = cy;

  return pose;
}

void WaypointNavigator::goalResponseCallback(const NavigateGoalHandle::SharedPtr& goal_handle) {
  if (!goal_handle) {
    RCLCPP_ERROR(this->get_logger(), "목표 거부됨");
  } else {
    RCLCPP_INFO(this->get_logger(), "목표 수락됨");
  }
}

void WaypointNavigator::feedbackCallback(const NavigateGoalHandle::SharedPtr& /*goal_handle*/, const std::shared_ptr<const NavigateAction::Feedback> feedback) {
  // Python 코드와 동일하게 피드백 처리
  if (current_waypoint_index_ < waypoints_.size()) {
    // 현재 위치와 목표까지의 거리 계산
    auto current_pose = feedback->current_pose.pose;

    // 목표 위치 (현재 웨이포인트 좌표)
    double target_x = waypoints_[current_waypoint_index_].x;
    double target_y = waypoints_[current_waypoint_index_].y;

    // 거리 계산 - Python 코드와 동일
    double dx = target_x - current_pose.position.x;
    double dy = target_y - current_pose.position.y;
    double distance = std::sqrt(dx * dx + dy * dy);

    RCLCPP_INFO(this->get_logger(), "현재 위치: x=%.2f, y=%.2f, 목표까지 거리: %.2fm", current_pose.position.x, current_pose.position.y, distance);
  }
}

void WaypointNavigator::resultCallback(const NavigateGoalHandle::WrappedResult& /* result */) {
  // 이 콜백은 async_get_result로 직접 결과를 받는 방식을 사용하기 때문에 비어 있음
  // navigateToWaypoints 함수에서 결과 처리
}

}  // namespace robot_navigation

// 메인 함수 정의 - 네임스페이스 밖에 위치
int main(int argc, char** argv) {
  rclcpp::init(argc, argv);

  auto waypoint_navigator = std::make_shared<robot_navigation::WaypointNavigator>();

  // 초기 위치 설정 (Python 코드와 동일하게)
  waypoint_navigator->setInitialPose(0.01, 0.0, 0.01, 1.0);

  // Python 코드와 동일한 웨이포인트 설정
  waypoint_navigator->setHardcodedWaypoints();

  // 웨이포인트 탐색 시작 (3초 대기 시간)
  waypoint_navigator->navigateToWaypoints(3.0);

  rclcpp::spin(waypoint_navigator);

  rclcpp::shutdown();
  return 0;
}