// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from receive_msg:msg/Receive.idl
// generated code does not contain a copyright notice

#ifndef RECEIVE_MSG__MSG__DETAIL__RECEIVE__TRAITS_HPP_
#define RECEIVE_MSG__MSG__DETAIL__RECEIVE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "receive_msg/msg/detail/receive__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace receive_msg
{

namespace msg
{

inline void to_flow_style_yaml(
  const Receive & msg,
  std::ostream & out)
{
  out << "{";
  // member: id
  {
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << ", ";
  }

  // member: speed
  {
    out << "speed: ";
    rosidl_generator_traits::value_to_yaml(msg.speed, out);
    out << ", ";
  }

  // member: position
  {
    out << "position: ";
    rosidl_generator_traits::value_to_yaml(msg.position, out);
    out << ", ";
  }

  // member: state
  {
    out << "state: ";
    rosidl_generator_traits::value_to_yaml(msg.state, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Receive & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << "\n";
  }

  // member: speed
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "speed: ";
    rosidl_generator_traits::value_to_yaml(msg.speed, out);
    out << "\n";
  }

  // member: position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "position: ";
    rosidl_generator_traits::value_to_yaml(msg.position, out);
    out << "\n";
  }

  // member: state
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "state: ";
    rosidl_generator_traits::value_to_yaml(msg.state, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Receive & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace receive_msg

namespace rosidl_generator_traits
{

[[deprecated("use receive_msg::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const receive_msg::msg::Receive & msg,
  std::ostream & out, size_t indentation = 0)
{
  receive_msg::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use receive_msg::msg::to_yaml() instead")]]
inline std::string to_yaml(const receive_msg::msg::Receive & msg)
{
  return receive_msg::msg::to_yaml(msg);
}

template<>
inline const char * data_type<receive_msg::msg::Receive>()
{
  return "receive_msg::msg::Receive";
}

template<>
inline const char * name<receive_msg::msg::Receive>()
{
  return "receive_msg/msg/Receive";
}

template<>
struct has_fixed_size<receive_msg::msg::Receive>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<receive_msg::msg::Receive>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<receive_msg::msg::Receive>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // RECEIVE_MSG__MSG__DETAIL__RECEIVE__TRAITS_HPP_
