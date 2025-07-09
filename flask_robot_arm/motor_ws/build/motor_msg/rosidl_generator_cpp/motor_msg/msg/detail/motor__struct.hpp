// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from motor_msg:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_MSG__MSG__DETAIL__MOTOR__STRUCT_HPP_
#define MOTOR_MSG__MSG__DETAIL__MOTOR__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__motor_msg__msg__Motor __attribute__((deprecated))
#else
# define DEPRECATED__motor_msg__msg__Motor __declspec(deprecated)
#endif

namespace motor_msg
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Motor_
{
  using Type = Motor_<ContainerAllocator>;

  explicit Motor_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = 0;
      this->speed = 0.0f;
      this->dir = 0;
      this->mode = 0;
      this->angle = 0.0f;
      this->state = 0;
      this->sub_divide = 0;
    }
  }

  explicit Motor_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = 0;
      this->speed = 0.0f;
      this->dir = 0;
      this->mode = 0;
      this->angle = 0.0f;
      this->state = 0;
      this->sub_divide = 0;
    }
  }

  // field types and members
  using _id_type =
    uint8_t;
  _id_type id;
  using _speed_type =
    float;
  _speed_type speed;
  using _dir_type =
    uint8_t;
  _dir_type dir;
  using _mode_type =
    uint8_t;
  _mode_type mode;
  using _angle_type =
    float;
  _angle_type angle;
  using _state_type =
    uint8_t;
  _state_type state;
  using _sub_divide_type =
    uint8_t;
  _sub_divide_type sub_divide;

  // setters for named parameter idiom
  Type & set__id(
    const uint8_t & _arg)
  {
    this->id = _arg;
    return *this;
  }
  Type & set__speed(
    const float & _arg)
  {
    this->speed = _arg;
    return *this;
  }
  Type & set__dir(
    const uint8_t & _arg)
  {
    this->dir = _arg;
    return *this;
  }
  Type & set__mode(
    const uint8_t & _arg)
  {
    this->mode = _arg;
    return *this;
  }
  Type & set__angle(
    const float & _arg)
  {
    this->angle = _arg;
    return *this;
  }
  Type & set__state(
    const uint8_t & _arg)
  {
    this->state = _arg;
    return *this;
  }
  Type & set__sub_divide(
    const uint8_t & _arg)
  {
    this->sub_divide = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    motor_msg::msg::Motor_<ContainerAllocator> *;
  using ConstRawPtr =
    const motor_msg::msg::Motor_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<motor_msg::msg::Motor_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<motor_msg::msg::Motor_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      motor_msg::msg::Motor_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<motor_msg::msg::Motor_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      motor_msg::msg::Motor_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<motor_msg::msg::Motor_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<motor_msg::msg::Motor_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<motor_msg::msg::Motor_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__motor_msg__msg__Motor
    std::shared_ptr<motor_msg::msg::Motor_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__motor_msg__msg__Motor
    std::shared_ptr<motor_msg::msg::Motor_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Motor_ & other) const
  {
    if (this->id != other.id) {
      return false;
    }
    if (this->speed != other.speed) {
      return false;
    }
    if (this->dir != other.dir) {
      return false;
    }
    if (this->mode != other.mode) {
      return false;
    }
    if (this->angle != other.angle) {
      return false;
    }
    if (this->state != other.state) {
      return false;
    }
    if (this->sub_divide != other.sub_divide) {
      return false;
    }
    return true;
  }
  bool operator!=(const Motor_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Motor_

// alias to use template instance with default allocator
using Motor =
  motor_msg::msg::Motor_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace motor_msg

#endif  // MOTOR_MSG__MSG__DETAIL__MOTOR__STRUCT_HPP_
