/**
 * @file /include/robot_vision/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date January 2025
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef robot_vision_QNODE_HPP_
#define robot_vision_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/
#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#endif
#include <QThread>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


#include <mutex>
#include <opencv2/opencv.hpp>

#include "robot_msgs/msg/master_msg.hpp"
#include "robot_msgs/msg/vision_msg.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "vision.hpp"
#include <QImage>
/*****************************************************************************
** Class
*****************************************************************************/
namespace robot_vision {

class QNode : public QThread {
  Q_OBJECT
 public:
  QNode();
  ~QNode();

  Vision robot_vision_;

  cv::Mat* imgRaw = NULL;   // 원본 이미지 저장
  bool isreceived = false;  // 이미지 수신 여부

 protected:
  void run();

 private:
  std::shared_ptr<rclcpp::Node> node;

  void initPubSub();

  // topic
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pubImage;
  rclcpp::Publisher<robot_msgs::msg::VisionMsg>::SharedPtr vision_pub;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub;

  void callbackImage(const sensor_msgs::msg::Image::SharedPtr msg_img);

 Q_SIGNALS:
  void rosShutDown();
  void sigRcvImg();
};

}  // namespace robot_vision

#endif /* robot_vision_QNODE_HPP_ */