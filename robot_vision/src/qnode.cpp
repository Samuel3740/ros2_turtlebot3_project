/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date January 2025
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include "../include/robot_vision/qnode.hpp"

#include <cv_bridge/cv_bridge.h>
namespace robot_vision {

QNode::QNode() {
  int argc = 0;
  char** argv = NULL;
  rclcpp::init(argc, argv);
  node = rclcpp::Node::make_shared("robot_vision");
  initPubSub();

  this->start();  // 소켓 초기화 후 스레드 시작
}

QNode::~QNode() {
  if (rclcpp::ok()) {
    rclcpp::shutdown();
  }
}

void QNode::run() {
  rclcpp::WallRate loop_rate(20);
  while (rclcpp::ok()) {
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }
  rclcpp::shutdown();
  Q_EMIT rosShutDown();
}

void QNode::initPubSub() {
  pubImage = node->create_publisher<sensor_msgs::msg::Image>("/camera/image", 10);
  vision_pub = node->create_publisher<robot_msgs::msg::VisionMsg>("turtle_vision", 100);
  image_sub = node->create_subscription<sensor_msgs::msg::Image>("camera1/image_raw", 10, std::bind(&QNode::callbackImage, this, std::placeholders::_1));
}

void QNode::callbackImage(const sensor_msgs::msg::Image::SharedPtr msg_img) {
  if (imgRaw == NULL && !isreceived)  // imgRaw -> NULL, isreceived -> false
  {
    try {
      // ROS2 이미지 메시지를 OpenCV Mat 형식으로 변환, 이미지 객체에 할당
      imgRaw = new cv::Mat(cv_bridge::toCvCopy(msg_img, sensor_msgs::image_encodings::RGB8)->image);

      if (imgRaw != NULL)  // imgRaw 변환 성공
      {
        Q_EMIT sigRcvImg();  // 이미지 수신을 알리는 시그널 발생
        isreceived = true;
      }
    } catch (cv_bridge::Exception& e) {
      RCLCPP_ERROR(node->get_logger(), "cv_bridge exception: %s", e.what());
      return;
    }
  }
}

}  // namespace robot_vision