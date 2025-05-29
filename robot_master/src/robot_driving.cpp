/**
 * @file /src/robot_driving.cpp
 *
 * @brief Implementation for robot driving control.
 *
 * @date May 2025
 **/

#include "../include/robot_master/robot_driving.hpp"

namespace robot_master {

bool RobotDriving::start = false;

RobotDriving::RobotDriving() : situation(Situation::NONE) {
  // Initialize motor values to 0
  motor_value_.linear.x = 0.0;
  motor_value_.linear.y = 0.0;
  motor_value_.linear.z = 0.0;
  motor_value_.angular.x = 0.0;
  motor_value_.angular.y = 0.0;
  motor_value_.angular.z = 0.0;

  // // Initialize simulation motor values
  // motor_value_sim_.twist.linear.x = 0.0;
  // motor_value_sim_.twist.linear.y = 0.0;
  // motor_value_sim_.twist.linear.z = 0.0;
  // motor_value_sim_.twist.angular.x = 0.0;
  // motor_value_sim_.twist.angular.y = 0.0;
  // motor_value_sim_.twist.angular.z = 0.0;

  // motor_value_sim_.header.frame_id = "base_link";
  // updateSimTimestamp();

  master_msg_.slam = false;
  master_msg_.qr = false;
  master_msg_.lift = false;
  master_msg_.item = "";
}

void RobotDriving::setSpeed(double linear, double angular) {
  motor_value_.linear.x = linear;
  motor_value_.angular.z = angular;

  // motor_value_sim_.twist.linear.x = linear;
  // motor_value_sim_.twist.angular.z = angular;

  // updateSimTimestamp();
}

// void RobotDriving::updateSimTimestamp() {
//   motor_value_sim_.header.stamp.sec = 0;
//   motor_value_sim_.header.stamp.nanosec = 0;
// }

// void RobotDriving::updateSimTimestampROS(const builtin_interfaces::msg::Time& time) { motor_value_sim_.header.stamp = time; }

void RobotDriving::go() {
  if (!start) {
    // If not started, ensure robot is stopped
    setSpeed(0.0, 0.0);
    return;
  }

  analyzeSituation();
}

void RobotDriving::analyzeSituation() {
  if (master_msg_.slam) {
    situation = Situation::SLAM;
  } else if (master_msg_.qr) {
    situation = Situation::RECOGNIZE_QR;
  } else if (master_msg_.lift) {
    situation = Situation::LIFT_PARCEL;
  }
}

}  // namespace robot_master