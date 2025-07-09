// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from receive_msg:msg/Receive.idl
// generated code does not contain a copyright notice

#ifndef RECEIVE_MSG__MSG__DETAIL__RECEIVE__STRUCT_HPP_
#define RECEIVE_MSG__MSG__DETAIL__RECEIVE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__receive_msg__msg__Receive __attribute__((deprecated))
#else
# define DEPRECATED__receive_msg__msg__Receive __declspec(deprecated)
#endif

namespace receive_msg
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Receive_
{
  using Type = Receive_<ContainerAllocator>;

  explicit Receive_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = 0;
      this->speed = 0.0f;
      this->position = 0.0f;
      this->state = 0;
    }
  }

  explicit Receive_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = 0;
      this->speed = 0.0f;
      this->position = 0.0f;
      this->state = 0;
    }
  }

  // field types and members
  using _id_type =
    uint8_t;
  _id_type id;
  using _speed_type =
    float;
  _speed_type speed;
  using _position_type =
    float;
  _position_type position;
  using _state_type =
    uint8_t;
  _state_type state;

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
  Type & set__position(
    const float & _arg)
  {
    this->position = _arg;
    return *this;
  }
  Type & set__state(
    const uint8_t & _arg)
  {
    this->state = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    receive_msg::msg::Receive_<ContainerAllocator> *;
  using ConstRawPtr =
    const receive_msg::msg::Receive_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<receive_msg::msg::Receive_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<receive_msg::msg::Receive_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      receive_msg::msg::Receive_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<receive_msg::msg::Receive_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      receive_msg::msg::Receive_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<receive_msg::msg::Receive_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<receive_msg::msg::Receive_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<receive_msg::msg::Receive_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__receive_msg__msg__Receive
    std::shared_ptr<receive_msg::msg::Receive_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__receive_msg__msg__Receive
    std::shared_ptr<receive_msg::msg::Receive_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Receive_ & other) const
  {
    if (this->id != other.id) {
      return false;
    }
    if (this->speed != other.speed) {
      return false;
    }
    if (this->position != other.position) {
      return false;
    }
    if (this->state != other.state) {
      return false;
    }
    return true;
  }
  bool operator!=(const Receive_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Receive_

// alias to use template instance with default allocator
using Receive =
  receive_msg::msg::Receive_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace receive_msg

#endif  // RECEIVE_MSG__MSG__DETAIL__RECEIVE__STRUCT_HPP_
