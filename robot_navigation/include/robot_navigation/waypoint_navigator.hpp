/**
 * @file /include/robot_navigation/waypoint_navigator.hpp
 *
 * @brief TurtleBot3 웨이포인트 네비게이션을 위한 헤더 - Python 코드 직접 변환
 *
 * @date May 2025
 **/

#ifndef ROBOT_NAVIGATION_WAYPOINT_NAVIGATOR_HPP
#define ROBOT_NAVIGATION_WAYPOINT_NAVIGATOR_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

namespace robot_navigation {

struct Waypoint {
  std::string name;
  double x;
  double y;
  double yaw;  // 라디안 단위
};

class WaypointNavigator : public rclcpp::Node {
 public:
  WaypointNavigator();
  virtual ~WaypointNavigator() = default;

  // 초기 위치 설정
  void setInitialPose(double x = 0.0, double y = 0.0, double z = 0.0, double w = 1.0);

  // Nav2 활성화 대기 (Python waitUntilNav2Active() 구현)
  bool waitUntilNav2Active();

  // 웨이포인트 로드 및 설정
  bool loadWaypointsFromFile(const std::string& filePath = "");
  void setWaypointsManually(const std::vector<Waypoint>& waypoints);

  // Python 코드와 동일한 웨이포인트 설정 (하드코딩)
  void setHardcodedWaypoints();

  // 웨이포인트 탐색 시작 (Python 코드 스타일로 구현)
  bool navigateToWaypoints(double waitTime = 3.0);

 private:
  // 웨이포인트 관련
  std::vector<Waypoint> waypoints_;
  std::vector<std::string> location_names_;

  // Nav2 액션 클라이언트
  using NavigateAction = nav2_msgs::action::NavigateToPose;
  using NavigateGoalHandle = rclcpp_action::ClientGoalHandle<NavigateAction>;
  rclcpp_action::Client<NavigateAction>::SharedPtr navigate_client_;

  // 현재 탐색 상태
  bool navigation_active_;
  size_t current_waypoint_index_;
  std::mutex navigation_mutex_;

  // 웨이포인트로부터 PoseStamped 생성
  geometry_msgs::msg::PoseStamped createPoseFromWaypoint(const Waypoint& waypoint);

  // 네비게이션 콜백 함수
  void goalResponseCallback(const NavigateGoalHandle::SharedPtr& goal_handle);
  void feedbackCallback(const NavigateGoalHandle::SharedPtr& /*goal_handle*/, const std::shared_ptr<const NavigateAction::Feedback> feedback);
  void resultCallback(const NavigateGoalHandle::WrappedResult& result);
};

}  // namespace robot_navigation

#endif  // ROBOT_NAVIGATION_WAYPOINT_NAVIGATOR_HPP