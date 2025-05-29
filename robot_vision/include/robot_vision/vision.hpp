#ifndef VISION_H
#define VISION_H

#include <cv_bridge/cv_bridge.h>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <robot_msgs/msg/master_msg.hpp>
#include <robot_msgs/msg/vision_msg.hpp>
#include "rclcpp/rclcpp.hpp"

namespace robot_vision {

class Vision : public QObject {
  Q_OBJECT

 public:
  Vision(QObject *parent = nullptr);
  ~Vision();

  static robot_msgs::msg::VisionMsg Vision_msg;
  static robot_msgs::msg::MasterMsg Master_msg;
  static int now_fps;

 Q_SIGNALS:

 private:
  cv::Mat Raw_image;
};

} // namespace robot_vision

#endif  // VISION_H
