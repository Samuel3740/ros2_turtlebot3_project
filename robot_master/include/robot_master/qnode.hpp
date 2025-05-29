/**
 * @file /include/robot_master/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date January 2025
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef robot_master_QNODE_HPP_
#define robot_master_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/
#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#endif
#include <QThread>
#include <opencv2/opencv.hpp>
#include <turtlebot3_msgs/msg/sensor_state.hpp>

#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "robot_driving.hpp"
#include "robot_msgs/msg/master_msg.hpp"
#include "robot_msgs/msg/vision_msg.hpp"
#include "robot_msgs/srv/navigate_to_parcel.hpp"
#include "std_msgs/msg/string.hpp"
#include "task_manager.hpp"
/*****************************************************************************
** Class
*****************************************************************************/
namespace robot_master {

extern bool button_clicked;
class QNode : public QThread {
  Q_OBJECT
 public:
  QNode();
  ~QNode();

  // ROS2 initialization
  void initPubSub();

  // Task management
  void setItemInfo(const std::string& item);
  void startFindParcelTask();
  void cancelTask();
  void navigateToPosition(double x, double y, double yaw);

  // Robot control
  void turtleRun();

  RobotDriving driving_;
  std::shared_ptr<TaskManager> task_manager_;

 protected:
  void run();

 private:
  std::shared_ptr<rclcpp::Node> node;

  // Publishers
  rclcpp::Publisher<robot_msgs::msg::MasterMsg>::SharedPtr pub_master;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_motor;
  //rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr pub_motor_sim;

  // Subscribers
  rclcpp::Subscription<robot_msgs::msg::VisionMsg>::SharedPtr sub_vision;

  rclcpp::Client<robot_msgs::srv::NavigateToParcel>::SharedPtr nav_client_;

  // Callback functions
  void visionCallback(const std::shared_ptr<robot_msgs::msg::VisionMsg> vision_msg);

 Q_SIGNALS:
  void rosShutDown();
  void dataReceived();
  void logMessage(const QString& message);
  void taskStateChanged(int state);
};

}  // namespace robot_master
#endif /* robot_master_QNODE_HPP_ */
