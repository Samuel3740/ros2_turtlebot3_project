/**
 * @file /include/robot_master/robot_driving.hpp
 *
 * @brief Header for robot driving control.
 *
 * @date May 2025
 **/

#ifndef ROBOT_MASTER_ROBOT_DRIVING_HPP
#define ROBOT_MASTER_ROBOT_DRIVING_HPP

#include <string>

#include "builtin_interfaces/msg/time.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "robot_msgs/msg/master_msg.hpp"

namespace robot_master {
enum class Situation { NONE, SLAM, SEARCH_PARCEL, RECOGNIZE_QR, NAVIGATION, LIFT_PARCEL, RETURN_TO_BASE };

class RobotDriving {
 public:
  RobotDriving();

  void go();
  void setSpeed(double linear, double angular);
  void analyzeSituation();

  // void updateSimTimestamp();
  // void updateSimTimestampROS(const builtin_interfaces::msg::Time& time);

  // Robot speed command
  geometry_msgs::msg::Twist motor_value_;

  // Simulated robot speed command
  //geometry_msgs::msg::TwistStamped motor_value_sim_;

  // Master message for communication
  robot_msgs::msg::MasterMsg master_msg_;

  // Static control flag
  static bool start;

 private:
  Situation situation;
};

}  // namespace robot_master

#endif  // ROBOT_MASTER_ROBOT_DRIVING_HPP