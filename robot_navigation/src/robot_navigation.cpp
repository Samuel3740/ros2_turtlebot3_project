/**
 * @file /src/robot_navigation.cpp
 *
 * @brief Implementation of the robot navigation node.
 *
 * @date May 2025
 **/

#include "robot_navigation/robot_navigation.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "yaml-cpp/yaml.h"

namespace robot_navigation {

RobotNavigation::RobotNavigation() : navigation_active_(false) {
  // 웨이포인트 네비게이터 초기화
  waypoint_navigator_ = std::make_shared<WaypointNavigator>();

  // 파셀 위치 로드
  loadParcelLocations();
}

bool RobotNavigation::navigateToParcel(int parcel_id) {
  if (navigation_active_) {
    return false;  // 이미 네비게이션 중
  }

  // 파셀 ID로 위치 찾기
  auto it = parcel_locations_.find(parcel_id);
  if (it == parcel_locations_.end()) {
    return false;  // 파셀 ID가 존재하지 않음
  }

  // 파셀 위치 추출
  double x = it->second.first;
  double y = it->second.second;

  // 웨이포인트 설정
  std::vector<Waypoint> waypoints;
  waypoints.push_back({"시작 위치", 0.01, 0.0, 0.0});
  waypoints.push_back({"파셀 위치", x, y, M_PI / 2});
  waypoints.push_back({"시작 위치 (귀환)", 0.01, 0.0, 0.0});

  // 웨이포인트 네비게이터 설정
  waypoint_navigator_->setInitialPose(0.01, 0.0, 0.01, 1.0);
  waypoint_navigator_->setWaypointsManually(waypoints);

  // 네비게이션 시작
  bool success = waypoint_navigator_->navigateToWaypoints();
  navigation_active_ = success;

  return success;
}

bool RobotNavigation::isNavigating() const { return navigation_active_; }

bool RobotNavigation::navigateToCoordinate(double x, double y, double theta) {
  if (navigation_active_) {
    return false;  // 이미 네비게이션 중
  }

  // 웨이포인트 설정
  std::vector<Waypoint> waypoints;
  waypoints.push_back({"시작 위치", 0.01, 0.0, 0.0});
  waypoints.push_back({"경유 위치 A", 0.65, 0.0, 0.0}); // 경유 위치 A 추가
  waypoints.push_back({"목표 위치", x, y, theta});
  waypoints.push_back({"경유 위치 A", 0.65, 0.0, 0.0}); // 경유 위치 A 추가
  waypoints.push_back({"시작 위치 (귀환)", 0.01, 0.0, 0.0});

  // 웨이포인트 네비게이터 설정
  waypoint_navigator_->setInitialPose(0.01, 0.0, 0.01, 1.0);
  waypoint_navigator_->setWaypointsManually(waypoints);

  // 네비게이션 시작
  bool success = waypoint_navigator_->navigateToWaypoints();
  navigation_active_ = success;

  return success;
}

void RobotNavigation::cancelNavigation() {
  // 웨이포인트 네비게이션을 취소하는 로직을 구현
  // (WaypointNavigator 클래스에 취소 메서드를 추가해야 함)

  navigation_active_ = false;
}

bool RobotNavigation::loadParcelLocations() {
  try {
    std::string config_path = ament_index_cpp::get_package_share_directory("robot_master") + "/config/parcel_locations.yaml";

    YAML::Node config = YAML::LoadFile(config_path);

    if (!config["parcels"] || !config["parcels"].IsSequence()) {
      return false;
    }

    parcel_locations_.clear();

    for (const auto& parcel_node : config["parcels"]) {
      int id = parcel_node["id"].as<int>();
      double x = parcel_node["x"].as<double>();
      double y = parcel_node["y"].as<double>();

      parcel_locations_[id] = std::make_pair(x, y);
    }

    return true;
  } catch (const std::exception& e) {
    // 파일이 없거나 로드 오류 발생 시 기본값 설정
    // 기본 파셀 위치 설정 (샘플)
    parcel_locations_[1] = std::make_pair(0.5, 0.5);
    parcel_locations_[2] = std::make_pair(0.8, 0.5);
    parcel_locations_[3] = std::make_pair(0.8, -0.5);
    parcel_locations_[4] = std::make_pair(0.5, -0.5);

    return true;
  }
}

}  // namespace robot_navigation