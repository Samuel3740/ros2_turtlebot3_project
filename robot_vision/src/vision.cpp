#include "../include/robot_vision/vision.hpp"

namespace robot_vision {

robot_msgs::msg::VisionMsg Vision::Vision_msg;
robot_msgs::msg::MasterMsg Vision::Master_msg;

int Vision::now_fps = 0;

Vision::Vision(QObject* parent) {
  // 생성자 구현
}

Vision::~Vision() {
  // 소멸자 구현
}


} // namespace robot_vision
