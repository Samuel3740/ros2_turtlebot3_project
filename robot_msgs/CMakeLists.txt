cmake_minimum_required(VERSION 3.8)
project(robot_msgs)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(action_msgs REQUIRED)

# 메시지, 서비스, 액션 파일 설정
set(msg_files
  "msg/MasterMsg.msg"
  "msg/VisionMsg.msg"
)

set(srv_files
  "srv/RecognizeItem.srv"
  "srv/NavigateToParcel.srv"
)

set(action_files
  "action/LiftAction.action"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  ${srv_files}
  ${action_files}
  DEPENDENCIES std_msgs geometry_msgs action_msgs
)
ament_export_dependencies(rosidl_default_runtime)

ament_package()
