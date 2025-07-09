// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from receive_msg:msg/Receive.idl
// generated code does not contain a copyright notice

#ifndef RECEIVE_MSG__MSG__DETAIL__RECEIVE__STRUCT_H_
#define RECEIVE_MSG__MSG__DETAIL__RECEIVE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/Receive in the package receive_msg.
typedef struct receive_msg__msg__Receive
{
  uint8_t id;
  float speed;
  float position;
  uint8_t state;
} receive_msg__msg__Receive;

// Struct for a sequence of receive_msg__msg__Receive.
typedef struct receive_msg__msg__Receive__Sequence
{
  receive_msg__msg__Receive * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} receive_msg__msg__Receive__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // RECEIVE_MSG__MSG__DETAIL__RECEIVE__STRUCT_H_
