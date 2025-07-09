// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from motor_msg:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_MSG__MSG__DETAIL__MOTOR__STRUCT_H_
#define MOTOR_MSG__MSG__DETAIL__MOTOR__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/Motor in the package motor_msg.
typedef struct motor_msg__msg__Motor
{
  uint8_t id;
  float speed;
  uint8_t dir;
  uint8_t mode;
  float angle;
  uint8_t state;
  uint8_t sub_divide;
} motor_msg__msg__Motor;

// Struct for a sequence of motor_msg__msg__Motor.
typedef struct motor_msg__msg__Motor__Sequence
{
  motor_msg__msg__Motor * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} motor_msg__msg__Motor__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MOTOR_MSG__MSG__DETAIL__MOTOR__STRUCT_H_
