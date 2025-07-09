// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from receive_msg:msg/Receive.idl
// generated code does not contain a copyright notice

#ifndef RECEIVE_MSG__MSG__DETAIL__RECEIVE__BUILDER_HPP_
#define RECEIVE_MSG__MSG__DETAIL__RECEIVE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "receive_msg/msg/detail/receive__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace receive_msg
{

namespace msg
{

namespace builder
{

class Init_Receive_state
{
public:
  explicit Init_Receive_state(::receive_msg::msg::Receive & msg)
  : msg_(msg)
  {}
  ::receive_msg::msg::Receive state(::receive_msg::msg::Receive::_state_type arg)
  {
    msg_.state = std::move(arg);
    return std::move(msg_);
  }

private:
  ::receive_msg::msg::Receive msg_;
};

class Init_Receive_position
{
public:
  explicit Init_Receive_position(::receive_msg::msg::Receive & msg)
  : msg_(msg)
  {}
  Init_Receive_state position(::receive_msg::msg::Receive::_position_type arg)
  {
    msg_.position = std::move(arg);
    return Init_Receive_state(msg_);
  }

private:
  ::receive_msg::msg::Receive msg_;
};

class Init_Receive_speed
{
public:
  explicit Init_Receive_speed(::receive_msg::msg::Receive & msg)
  : msg_(msg)
  {}
  Init_Receive_position speed(::receive_msg::msg::Receive::_speed_type arg)
  {
    msg_.speed = std::move(arg);
    return Init_Receive_position(msg_);
  }

private:
  ::receive_msg::msg::Receive msg_;
};

class Init_Receive_id
{
public:
  Init_Receive_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Receive_speed id(::receive_msg::msg::Receive::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_Receive_speed(msg_);
  }

private:
  ::receive_msg::msg::Receive msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::receive_msg::msg::Receive>()
{
  return receive_msg::msg::builder::Init_Receive_id();
}

}  // namespace receive_msg

#endif  // RECEIVE_MSG__MSG__DETAIL__RECEIVE__BUILDER_HPP_
