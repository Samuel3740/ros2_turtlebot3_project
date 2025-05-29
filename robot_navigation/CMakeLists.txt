cmake_minimum_required(VERSION 3.8)
project(robot_navigation)

# C++ 표준 설정
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 의존성 패키지 찾기
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(robot_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(ament_index_cpp REQUIRED)

# 헤더 파일 경로 추가
include_directories(include)

# 네비게이션 노드 라이브러리
add_library(${PROJECT_NAME} SHARED
  src/robot_navigation.cpp
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rclcpp_action
  geometry_msgs
  nav2_msgs
  robot_msgs
  tf2
  tf2_geometry_msgs
  yaml-cpp
  ament_index_cpp
)

# 웨이포인트 네비게이터 실행 파일
add_executable(waypoint_navigator
  src/waypoint_navigator.cpp
)

ament_target_dependencies(waypoint_navigator
  rclcpp
  rclcpp_action
  geometry_msgs
  nav2_msgs
  tf2
  tf2_geometry_msgs
  yaml-cpp
  ament_index_cpp
)

# YAML-CPP 라이브러리 명시적 링크 추가
target_link_libraries(waypoint_navigator
  ${PROJECT_NAME}
  ${YAML_CPP_LIBRARIES}
)

# 설정 파일 및 런치 파일 설치
install(DIRECTORY
  config
  launch
  DESTINATION share/${PROJECT_NAME}
)

# 실행 파일 설치
install(TARGETS
  ${PROJECT_NAME}
  waypoint_navigator
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

# 헤더 파일 설치
install(DIRECTORY include/
  DESTINATION include/
)

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(
  rclcpp
  rclcpp_action
  geometry_msgs
  nav2_msgs
  robot_msgs
  tf2
  tf2_geometry_msgs
  yaml-cpp
  ament_index_cpp
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()