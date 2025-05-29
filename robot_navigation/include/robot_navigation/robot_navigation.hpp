// Modified version of robot_navigation.hpp to integrate with waypoint_navigator

/**
 * @file /include/robot_navigation/robot_navigation.hpp
 *
 * @brief Header for the robot navigation node.
 *
 * @date May 2025
 **/

#ifndef ROBOT_NAVIGATION_ROBOT_NAVIGATION_HPP
#define ROBOT_NAVIGATION_ROBOT_NAVIGATION_HPP

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "robot_msgs/srv/navigate_to_parcel.hpp"
#include "robot_navigation/waypoint_navigator.hpp"

namespace robot_navigation {

class RobotNavigation {
 public:
  RobotNavigation();
  virtual ~RobotNavigation() = default;

  // 현재 파셀 위치로 네비게이션
  bool navigateToParcel(int parcel_id);

  // 네비게이션 상태 확인
  bool isNavigating() const;

  // 특정 좌표로 이동
  bool navigateToCoordinate(double x, double y, double theta);

  // 현재 작업 취소
  void cancelNavigation();

 private:
  // 웨이포인트 네비게이터 인스턴스
  std::shared_ptr<WaypointNavigator> waypoint_navigator_;

  // 파셀 위치 관리
  bool loadParcelLocations();
  std::map<int, std::pair<double, double>> parcel_locations_;
  bool navigation_active_;
};

}  // namespace robot_navigation

#endif  // ROBOT_NAVIGATION_ROBOT_NAVIGATION_HPP