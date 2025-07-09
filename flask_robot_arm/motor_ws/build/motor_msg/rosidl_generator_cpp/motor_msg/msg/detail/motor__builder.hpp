// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from motor_msg:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_MSG__MSG__DETAIL__MOTOR__BUILDER_HPP_
#define MOTOR_MSG__MSG__DETAIL__MOTOR__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "motor_msg/msg/detail/motor__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace motor_msg
{

namespace msg
{

namespace builder
{

class Init_Motor_sub_divide
{
public:
  explicit Init_Motor_sub_divide(::motor_msg::msg::Motor & msg)
  : msg_(msg)
  {}
  ::motor_msg::msg::Motor sub_divide(::motor_msg::msg::Motor::_sub_divide_type arg)
  {
    msg_.sub_divide = std::move(arg);
    return std::move(msg_);
  }

private:
  ::motor_msg::msg::Motor msg_;
};

class Init_Motor_state
{
public:
  explicit Init_Motor_state(::motor_msg::msg::Motor & msg)
  : msg_(msg)
  {}
  Init_Motor_sub_divide state(::motor_msg::msg::Motor::_state_type arg)
  {
    msg_.state = std::move(arg);
    return Init_Motor_sub_divide(msg_);
  }

private:
  ::motor_msg::msg::Motor msg_;
};

class Init_Motor_angle
{
public:
  explicit Init_Motor_angle(::motor_msg::msg::Motor & msg)
  : msg_(msg)
  {}
  Init_Motor_state angle(::motor_msg::msg::Motor::_angle_type arg)
  {
    msg_.angle = std::move(arg);
    return Init_Motor_state(msg_);
  }

private:
  ::motor_msg::msg::Motor msg_;
};

class Init_Motor_mode
{
public:
  explicit Init_Motor_mode(::motor_msg::msg::Motor & msg)
  : msg_(msg)
  {}
  Init_Motor_angle mode(::motor_msg::msg::Motor::_mode_type arg)
  {
    msg_.mode = std::move(arg);
    return Init_Motor_angle(msg_);
  }

private:
  ::motor_msg::msg::Motor msg_;
};

class Init_Motor_dir
{
public:
  explicit Init_Motor_dir(::motor_msg::msg::Motor & msg)
  : msg_(msg)
  {}
  Init_Motor_mode dir(::motor_msg::msg::Motor::_dir_type arg)
  {
    msg_.dir = std::move(arg);
    return Init_Motor_mode(msg_);
  }

private:
  ::motor_msg::msg::Motor msg_;
};

class Init_Motor_speed
{
public:
  explicit Init_Motor_speed(::motor_msg::msg::Motor & msg)
  : msg_(msg)
  {}
  Init_Motor_dir speed(::motor_msg::msg::Motor::_speed_type arg)
  {
    msg_.speed = std::move(arg);
    return Init_Motor_dir(msg_);
  }

private:
  ::motor_msg::msg::Motor msg_;
};

class Init_Motor_id
{
public:
  Init_Motor_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Motor_speed id(::motor_msg::msg::Motor::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_Motor_speed(msg_);
  }

private:
  ::motor_msg::msg::Motor msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::motor_msg::msg::Motor>()
{
  return motor_msg::msg::builder::Init_Motor_id();
}

}  // namespace motor_msg

#endif  // MOTOR_MSG__MSG__DETAIL__MOTOR__BUILDER_HPP_
