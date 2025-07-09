// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from receive_msg:msg/Receive.idl
// generated code does not contain a copyright notice
#include "receive_msg/msg/detail/receive__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
receive_msg__msg__Receive__init(receive_msg__msg__Receive * msg)
{
  if (!msg) {
    return false;
  }
  // id
  // speed
  // position
  // state
  return true;
}

void
receive_msg__msg__Receive__fini(receive_msg__msg__Receive * msg)
{
  if (!msg) {
    return;
  }
  // id
  // speed
  // position
  // state
}

bool
receive_msg__msg__Receive__are_equal(const receive_msg__msg__Receive * lhs, const receive_msg__msg__Receive * rhs)
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
  // position
  if (lhs->position != rhs->position) {
    return false;
  }
  // state
  if (lhs->state != rhs->state) {
    return false;
  }
  return true;
}

bool
receive_msg__msg__Receive__copy(
  const receive_msg__msg__Receive * input,
  receive_msg__msg__Receive * output)
{
  if (!input || !output) {
    return false;
  }
  // id
  output->id = input->id;
  // speed
  output->speed = input->speed;
  // position
  output->position = input->position;
  // state
  output->state = input->state;
  return true;
}

receive_msg__msg__Receive *
receive_msg__msg__Receive__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  receive_msg__msg__Receive * msg = (receive_msg__msg__Receive *)allocator.allocate(sizeof(receive_msg__msg__Receive), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(receive_msg__msg__Receive));
  bool success = receive_msg__msg__Receive__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
receive_msg__msg__Receive__destroy(receive_msg__msg__Receive * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    receive_msg__msg__Receive__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
receive_msg__msg__Receive__Sequence__init(receive_msg__msg__Receive__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  receive_msg__msg__Receive * data = NULL;

  if (size) {
    data = (receive_msg__msg__Receive *)allocator.zero_allocate(size, sizeof(receive_msg__msg__Receive), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = receive_msg__msg__Receive__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        receive_msg__msg__Receive__fini(&data[i - 1]);
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
receive_msg__msg__Receive__Sequence__fini(receive_msg__msg__Receive__Sequence * array)
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
      receive_msg__msg__Receive__fini(&array->data[i]);
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

receive_msg__msg__Receive__Sequence *
receive_msg__msg__Receive__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  receive_msg__msg__Receive__Sequence * array = (receive_msg__msg__Receive__Sequence *)allocator.allocate(sizeof(receive_msg__msg__Receive__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = receive_msg__msg__Receive__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
receive_msg__msg__Receive__Sequence__destroy(receive_msg__msg__Receive__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    receive_msg__msg__Receive__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
receive_msg__msg__Receive__Sequence__are_equal(const receive_msg__msg__Receive__Sequence * lhs, const receive_msg__msg__Receive__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!receive_msg__msg__Receive__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
receive_msg__msg__Receive__Sequence__copy(
  const receive_msg__msg__Receive__Sequence * input,
  receive_msg__msg__Receive__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(receive_msg__msg__Receive);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    receive_msg__msg__Receive * data =
      (receive_msg__msg__Receive *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!receive_msg__msg__Receive__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          receive_msg__msg__Receive__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!receive_msg__msg__Receive__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
