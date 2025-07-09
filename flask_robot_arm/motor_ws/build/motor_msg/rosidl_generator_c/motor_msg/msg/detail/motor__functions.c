// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from motor_msg:msg/Motor.idl
// generated code does not contain a copyright notice
#include "motor_msg/msg/detail/motor__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
motor_msg__msg__Motor__init(motor_msg__msg__Motor * msg)
{
  if (!msg) {
    return false;
  }
  // id
  // speed
  // dir
  // mode
  // angle
  // state
  // sub_divide
  return true;
}

void
motor_msg__msg__Motor__fini(motor_msg__msg__Motor * msg)
{
  if (!msg) {
    return;
  }
  // id
  // speed
  // dir
  // mode
  // angle
  // state
  // sub_divide
}

bool
motor_msg__msg__Motor__are_equal(const motor_msg__msg__Motor * lhs, const motor_msg__msg__Motor * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // id
  if (lhs->id != rhs->id) {
    return false;
  }
  // speed
  if (lhs->speed != rhs->speed) {
    return false;
  }
  // dir
  if (lhs->dir != rhs->dir) {
    return false;
  }
  // mode
  if (lhs->mode != rhs->mode) {
    return false;
  }
  // angle
  if (lhs->angle != rhs->angle) {
    return false;
  }
  // state
  if (lhs->state != rhs->state) {
    return false;
  }
  // sub_divide
  if (lhs->sub_divide != rhs->sub_divide) {
    return false;
  }
  return true;
}

bool
motor_msg__msg__Motor__copy(
  const motor_msg__msg__Motor * input,
  motor_msg__msg__Motor * output)
{
  if (!input || !output) {
    return false;
  }
  // id
  output->id = input->id;
  // speed
  output->speed = input->speed;
  // dir
  output->dir = input->dir;
  // mode
  output->mode = input->mode;
  // angle
  output->angle = input->angle;
  // state
  output->state = input->state;
  // sub_divide
  output->sub_divide = input->sub_divide;
  return true;
}

motor_msg__msg__Motor *
motor_msg__msg__Motor__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_msg__msg__Motor * msg = (motor_msg__msg__Motor *)allocator.allocate(sizeof(motor_msg__msg__Motor), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(motor_msg__msg__Motor));
  bool success = motor_msg__msg__Motor__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
motor_msg__msg__Motor__destroy(motor_msg__msg__Motor * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    motor_msg__msg__Motor__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
motor_msg__msg__Motor__Sequence__init(motor_msg__msg__Motor__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_msg__msg__Motor * data = NULL;

  if (size) {
    data = (motor_msg__msg__Motor *)allocator.zero_allocate(size, sizeof(motor_msg__msg__Motor), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = motor_msg__msg__Motor__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        motor_msg__msg__Motor__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
motor_msg__msg__Motor__Sequence__fini(motor_msg__msg__Motor__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      motor_msg__msg__Motor__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

motor_msg__msg__Motor__Sequence *
motor_msg__msg__Motor__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_msg__msg__Motor__Sequence * array = (motor_msg__msg__Motor__Sequence *)allocator.allocate(sizeof(motor_msg__msg__Motor__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = motor_msg__msg__Motor__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
motor_msg__msg__Motor__Sequence__destroy(motor_msg__msg__Motor__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    motor_msg__msg__Motor__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
motor_msg__msg__Motor__Sequence__are_equal(const motor_msg__msg__Motor__Sequence * lhs, const motor_msg__msg__Motor__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!motor_msg__msg__Motor__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
motor_msg__msg__Motor__Sequence__copy(
  const motor_msg__msg__Motor__Sequence * input,
  motor_msg__msg__Motor__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(motor_msg__msg__Motor);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    motor_msg__msg__Motor * data =
      (motor_msg__msg__Motor *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!motor_msg__msg__Motor__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          motor_msg__msg__Motor__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!motor_msg__msg__Motor__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
