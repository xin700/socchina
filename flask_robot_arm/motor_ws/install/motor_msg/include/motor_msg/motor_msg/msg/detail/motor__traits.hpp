// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from motor_msg:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_MSG__MSG__DETAIL__MOTOR__TRAITS_HPP_
#define MOTOR_MSG__MSG__DETAIL__MOTOR__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "motor_msg/msg/detail/motor__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace motor_msg
{

namespace msg
{

inline void to_flow_style_yaml(
  const Motor & msg,
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

  // member: dir
  {
    out << "dir: ";
    rosidl_generator_traits::value_to_yaml(msg.dir, out);
    out << ", ";
  }

  // member: mode
  {
    out << "mode: ";
    rosidl_generator_traits::value_to_yaml(msg.mode, out);
    out << ", ";
  }

  // member: angle
  {
    out << "angle: ";
    rosidl_generator_traits::value_to_yaml(msg.angle, out);
    out << ", ";
  }

  // member: state
  {
    out << "state: ";
    rosidl_generator_traits::value_to_yaml(msg.state, out);
    out << ", ";
  }

  // member: sub_divide
  {
    out << "sub_divide: ";
    rosidl_generator_traits::value_to_yaml(msg.sub_divide, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Motor & msg,
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

  // member: dir
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "dir: ";
    rosidl_generator_traits::value_to_yaml(msg.dir, out);
    out << "\n";
  }

  // member: mode
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "mode: ";
    rosidl_generator_traits::value_to_yaml(msg.mode, out);
    out << "\n";
  }

  // member: angle
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "angle: ";
    rosidl_generator_traits::value_to_yaml(msg.angle, out);
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

  // member: sub_divide
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "sub_divide: ";
    rosidl_generator_traits::value_to_yaml(msg.sub_divide, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Motor & msg, bool use_flow_style = false)
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

}  // namespace motor_msg

namespace rosidl_generator_traits
{

[[deprecated("use motor_msg::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const motor_msg::msg::Motor & msg,
  std::ostream & out, size_t indentation = 0)
{
  motor_msg::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use motor_msg::msg::to_yaml() instead")]]
inline std::string to_yaml(const motor_msg::msg::Motor & msg)
{
  return motor_msg::msg::to_yaml(msg);
}

template<>
inline const char * data_type<motor_msg::msg::Motor>()
{
  return "motor_msg::msg::Motor";
}

template<>
inline const char * name<motor_msg::msg::Motor>()
{
  return "motor_msg/msg/Motor";
}

template<>
struct has_fixed_size<motor_msg::msg::Motor>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<motor_msg::msg::Motor>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<motor_msg::msg::Motor>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MOTOR_MSG__MSG__DETAIL__MOTOR__TRAITS_HPP_
