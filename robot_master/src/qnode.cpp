/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date May 2025
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include "../include/robot_master/qnode.hpp"

namespace robot_master {

bool button_clicked = false;
QNode::QNode() {
  int argc = 0;
  char** argv = NULL;
  rclcpp::init(argc, argv);
  node = rclcpp::Node::make_shared("robot_master");
  initPubSub();

  // Initialize the task manager
  task_manager_ = std::make_shared<TaskManager>(node);

  this->start();
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

    turtleRun();
    Q_EMIT dataReceived();
    // Update UI with task state if changed
    static TaskState last_state = TaskState::IDLE;
    TaskState current_state = task_manager_->getCurrentState();

    if (current_state != last_state) {
      Q_EMIT taskStateChanged(static_cast<int>(current_state));
      last_state = current_state;

      // Log the state change
      QString state_msg = QString("Task state changed to: ");
      switch (current_state) {
        case TaskState::IDLE:
          state_msg += "IDLE";
          break;
        case TaskState::SEARCHING:
          state_msg += "SEARCHING";
          break;
        case TaskState::NAVIGATING_TO_PARCEL:
          state_msg += "NAVIGATING_TO_PARCEL";
          break;
        case TaskState::RECOGNIZING_PARCEL:
          state_msg += "RECOGNIZING_PARCEL";
          break;
        case TaskState::LIFTING_PARCEL:
          state_msg += "LIFTING_PARCEL";
          break;
        case TaskState::DELIVERING:
          state_msg += "DELIVERING";
          break;
        case TaskState::DROPPING_PARCEL:
          state_msg += "DROPPING_PARCEL";
          break;
        case TaskState::COMPLETED:
          state_msg += "COMPLETED";
          break;
        case TaskState::ERROR:
          state_msg += "ERROR";
          break;
      }

      Q_EMIT logMessage(state_msg);
    }

    loop_rate.sleep();
  }
  rclcpp::shutdown();
  Q_EMIT rosShutDown();
}

void QNode::initPubSub() {
  pub_master = node->create_publisher<robot_msgs::msg::MasterMsg>("turtle_master", 100);
  pub_motor = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  //pub_motor_sim = node->create_publisher<geometry_msgs::msg::TwistStamped>("sim_cmd_vel", 10);
  sub_vision = node->create_subscription<robot_msgs::msg::VisionMsg>("turtle_vision", 100, std::bind(&QNode::visionCallback, this, std::placeholders::_1));
  nav_client_ = node->create_client<robot_msgs::srv::NavigateToParcel>("navigate_to_parcel");

  // 시뮬레이션 모드 파라미터 추가 (기본값: true)
  node->declare_parameter("sim_mode", true);
}

void QNode::visionCallback(const std::shared_ptr<robot_msgs::msg::VisionMsg> vision_msg) {
  if (vision_msg) {
    // Log QR code detection if found
    if (vision_msg->qr_detected) {
      QString qr_msg = QString("QR Code detected: %1").arg(QString::fromStdString(vision_msg->qr_data));
      Q_EMIT logMessage(qr_msg);
    }
  }
}

void QNode::setItemInfo(const std::string& item) { driving_.master_msg_.item = item; }

void QNode::startFindParcelTask() {
  if (driving_.master_msg_.item.empty()) {
    Q_EMIT logMessage("Cannot start task: No item ID specified");
    return;
  }

  task_manager_->startFindParcelTask(driving_.master_msg_.item);
}

void QNode::cancelTask() {
  Q_EMIT logMessage("Canceling current task");
  task_manager_->cancelCurrentTask();
}

void QNode::turtleRun() {
  driving_.go();

  pub_motor->publish(driving_.motor_value_);

  bool sim_mode = true;
  node->get_parameter("sim_mode", sim_mode);

  // if (sim_mode) {
  //   builtin_interfaces::msg::Time current_time = node->now();
  //   driving_.updateSimTimestampROS(current_time);

  //   pub_motor_sim->publish(driving_.motor_value_sim_);

  //   RCLCPP_DEBUG(node->get_logger(), "시뮬레이션 명령: linear.x=%.2f, angular.z=%.2f", driving_.motor_value_sim_.twist.linear.x, driving_.motor_value_sim_.twist.angular.z);
  // }

  pub_master->publish(driving_.master_msg_);

  if (driving_.master_msg_.slam) {
    RCLCPP_INFO(node->get_logger(), "Setting navigation goal (for debugging): x=1.0, y=2.0, yaw=90.0");
    driving_.master_msg_.slam = false;  // Reset flag to prevent repeated goals
  }
}

void QNode::navigateToPosition(double x, double y, double yaw) {
  if (!nav_client_->wait_for_service(std::chrono::seconds(1))) {
    Q_EMIT logMessage("내비게이션 서비스를 사용할 수 없습니다");
    return;
  }

  auto request = std::make_shared<robot_msgs::srv::NavigateToParcel::Request>();
  request->x = x;
  request->y = y;
  request->yaw = yaw;

  Q_EMIT logMessage(QString("내비게이션 요청 전송: x=%.2f, y=%.2f, yaw=%.2f").arg(x).arg(y).arg(yaw));

  auto future = nav_client_->async_send_request(request, [this](rclcpp::Client<robot_msgs::srv::NavigateToParcel>::SharedFuture future) {
    auto response = future.get();
    QString result_msg = QString("내비게이션 결과: %1").arg(response->success ? "성공" : "실패");
    if (!response->message.empty()) {
      result_msg += QString(" - %1").arg(QString::fromStdString(response->message));
    }
    Q_EMIT logMessage(result_msg);
  });
}

}  // namespace robot_master