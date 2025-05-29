/**
 * @file /src/main_navigation.cpp
 *
 * @brief Main entry point for the navigation node with waypoint navigator integration.
 *
 * @date May 2025
 **/

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "robot_msgs/srv/navigate_to_parcel.hpp"
#include "robot_navigation/robot_navigation.hpp"
#include "robot_navigation/waypoint_navigator.hpp"

/**
 * @brief Main entry point for the navigation node
 */
class NavigationNode : public rclcpp::Node {
 public:
  NavigationNode() : Node("navigation_node") {
    // 웨이포인트 네비게이터 생성
    waypoint_navigator_ = std::make_shared<robot_navigation::WaypointNavigator>();

    // 파셀 네비게이션 서비스 생성
    navigate_service_ =
        this->create_service<robot_msgs::srv::NavigateToParcel>("navigate_to_parcel", std::bind(&NavigationNode::handleNavigationRequest, this, std::placeholders::_1, std::placeholders::_2));

    RCLCPP_INFO(this->get_logger(), "네비게이션 노드가 시작되었습니다.");
    RCLCPP_INFO(this->get_logger(), "서비스 준비 완료: navigate_to_parcel");
  }

 private:
  std::shared_ptr<robot_navigation::WaypointNavigator> waypoint_navigator_;
  rclcpp::Service<robot_msgs::srv::NavigateToParcel>::SharedPtr navigate_service_;

  void handleNavigationRequest(const std::shared_ptr<robot_msgs::srv::NavigateToParcel::Request> request, std::shared_ptr<robot_msgs::srv::NavigateToParcel::Response> response) {
    RCLCPP_INFO(this->get_logger(), "파셀 ID %d로 네비게이션 요청 받음", request->parcel_id);

    // 초기 포즈 설정 (기본값 사용)
    waypoint_navigator_->setInitialPose();

    // 파셀 ID에 따라 적절한 웨이포인트 생성
    // 이 부분은 실제 애플리케이션에 맞게 조정 필요
    std::vector<robot_navigation::Waypoint> waypoints;

    // 파셀 위치 정보를 로드하거나 계산
    // 여기서는 간단한 예시만 제공
    double target_x = 0.5 + (request->parcel_id * 0.1);
    double target_y = 0.3 + (request->parcel_id * 0.1);

    waypoints.push_back({"시작 위치", 0.01, 0.0, 0.0});
    waypoints.push_back({"파셀 위치", target_x, target_y, M_PI / 2});
    waypoints.push_back({"시작 위치 (귀환)", 0.01, 0.0, 0.0});

    // 웨이포인트 설정
    waypoint_navigator_->setWaypointsManually(waypoints);

    // 네비게이션 시작
    bool success = waypoint_navigator_->navigateToWaypoints();

    // 응답 설정
    response->success = success;
    response->message = success ? "네비게이션 시작됨" : "네비게이션 시작 실패";
  }
};

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);

  auto node = std::make_shared<NavigationNode>();

  rclcpp::spin(node);
  rclcpp::shutdown();

  return 0;
}